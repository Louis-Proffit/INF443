#include "heightmap.hpp"

#include <random>
#include <iostream>

using namespace vcl;

std::vector<std::vector<float>> generateRandomHeightData(const random_heightmap_parameters &params)
{

    std::vector<std::vector<float>> heightData(params.rows, std::vector<float>(params.columns, 0.0f)); // Constructor de la class vector pour avoir un vector de vectors (matrice) tous initialisés à 0

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> hillRadiusDistribution(params.hill_radius_min, params.hill_radius_max);
    std::uniform_real_distribution<> hillHeightDistribution(params.hill_min_height, params.hill_max_height);
    std::uniform_int_distribution<> hillCenterRowIntDistribution(0, params.rows - 1);
    std::uniform_int_distribution<> hillCenterColIntDistribution(0, params.columns - 1);

    for (int i = 0; i < params.numHills; i++)
    {
        int hillCenterRow = hillCenterRowIntDistribution(generator);
        int hillCenterCol = hillCenterColIntDistribution(generator);
        int hillRadius = hillRadiusDistribution(generator);
        float hillHeight = hillHeightDistribution(generator);

        for (int r = hillCenterRow - hillRadius; r < hillCenterRow + hillRadius; r++)
        {
            for (int c = hillCenterCol - hillRadius; c < hillCenterCol + hillRadius; c++)
            {
                if (r < 0 || r >= params.rows || c < 0 || c >= params.columns)
                {
                    continue;
                }
                int r2 = hillRadius * hillRadius; // r*r term
                int x2x1 = hillCenterCol - c;     // (x2-x1) term
                int y2y1 = hillCenterRow - r;     // (y2-y1) term
                float height = float(r2 - x2x1 * x2x1 - y2y1 * y2y1);
                if (height < 0.0f)
                {
                    continue;
                }
                float factor = height / r2;
                heightData[r][c] = std::max(hillHeight * factor, heightData[r][c]);
                if (heightData[r][c] > 1.0f)
                {
                    heightData[r][c] = 1.0f;
                }
            }
        }
    }
    return heightData;
}

mesh createFromHeightData(const std::vector<std::vector<float>> &heightData, heightmap_parameters& params)
{
    int rows = heightData.size();
    int columns = heightData[0].size();
    params.rows = rows;
    params.columns = columns;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(rows * columns);
    terrain.uv.resize(rows * columns);

    // Fill terrain geometry
    float u, v, x, y;
    vec3 p;
    for (unsigned int ku = 0; ku < rows; ++ku)
    {
        for (unsigned int kv = 0; kv < columns; ++kv)
        {
            vec3 p;
            u = ku / (rows - 1.0f);
            v = kv / (columns - 1.0f);
            x = params.min_x + u * (params.max_x - params.min_x);
            y = params.min_y + v * (params.max_y - params.min_y);
            if ((ku > 0) && (ku < rows - 1) && (kv > 0) && (kv < columns - 1))
            {
                p = vec3(x, y, 
                    (heightData[ku][kv] + heightData[ku - 1][kv - 1] + heightData[ku - 1][kv] + heightData[ku - 1][kv + 1] +
                     heightData[ku][kv - 1] + heightData[ku][kv + 1] + heightData[ku + 1][kv - 1] + heightData[ku + 1][kv] +
                     heightData[ku + 1][kv + 1]) / 9);
            }
            else
            {
                p = vec3(x, y, heightData[ku][kv]);
            }

            // Store vertex coordinates
            terrain.position[kv + columns * ku] = p;
            terrain.uv[kv + columns * ku] = {x, y};
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for (size_t ku = 0; ku < rows - 1; ++ku)
    {
        for (size_t kv = 0; kv < columns - 1; ++kv)
        {
            const unsigned int idx = kv + columns * ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx + 1 + columns, idx + 1};
            const uint3 triangle_2 = {idx, idx + columns, idx + 1 + columns};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    terrain.fill_empty_field(); // need to call this function to fill the other buffer with default values (normal, color, etc)
    return terrain;
}

std::vector<std::vector<float>> generateFileHeightData(const std::string &filename, float horizontal_scale)
{
    image_raw im = image_load_png(filename);

    int rows = im.height;
    int columns = im.width;
    size_t const N = size_t(im.width) * size_t(im.height);
    float min = 1;
    float max = 0;
    vec3 p;
    std::vector<std::vector<float>> heightData(rows, std::vector<float>(columns, 0.0f));
    if (im.color_type == image_color_type::rgb)
    {
        for (size_t k = 0; k < N; ++k)
        {
            p = vec3(im.data[3 * k + 0], im.data[3 * k + 1], im.data[3 * k + 2]) / 255.0f;
            min = std::min(min, p.x);
            max = std::max(max, p.x);
        }
    }
    else if (im.color_type == image_color_type::rgba)
    {
        for (size_t k = 0; k < N; ++k)
        {
            p = vec3(im.data[4 * k + 0], im.data[4 * k + 1], im.data[4 * k + 2]) / 255.0f;
            //std::cout << p << std::endl;
            if (p.x < min)
            {
                min = p.x;
            }
            if (p.x > max)
            {
                max = p.x;
            }
        }
        /* // Pour le debugging
        std::cout << min << std::endl;
        std::cout << max << std::endl;*/
    }
    if (im.color_type == image_color_type::rgb)
    {
        for (unsigned int ku = 0; ku < rows; ++ku)
        {
            for (unsigned int kv = 0; kv < columns; ++kv)
            {
                p = vec3(im.data[3 * (ku + kv) + 0], im.data[3 * (ku + kv) + 1], im.data[3 * (ku + kv) + 2]) / 255.0f;
                heightData[ku][kv] = ((p.x - min) / (max - min)) * horizontal_scale;
            }
        }
    }
    else if (im.color_type == image_color_type::rgba)
    {
        for (unsigned int ku = 0; ku < rows; ++ku)
        {
            for (unsigned int kv = 0; kv < columns; ++kv)
            {
                p = vec3(im.data[4 * (columns * ku + kv) + 0], im.data[4 * (columns * ku + kv) + 1], im.data[4 * (columns * ku + kv) + 2]) / 255.0f;
                //std::cout << p.x << std::endl;
                heightData[ku][kv] = (p.x - min) / (max - min) * horizontal_scale;
                //std::cout << heightData[ku][kv] << std::endl;
            }
        }
    }
    return heightData;
}