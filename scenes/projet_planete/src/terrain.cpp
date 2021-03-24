#include "terrain.hpp"

using namespace vcl;

const unsigned int grid_steps = 10;
const float planet_radius = 1.425f;
const float corrected_radius = sqrt(pow(planet_radius, 2) - 2);
const float perturbation_shift_proportion = 0.9f;
const float spacing = 0.0f;
const vec3 color_grey_low = { float(90) / 255, float(90) / 255, float(90) / 255};
const vec3 color_grey_high = { float(30) / 255, float(30) / 255, float(30) / 255 };
const vec3 color_blue_low = { float(0) / 255, float(191) / 255, float(255) / 255 };
const vec3 color_blue_high = { float(30) / 255, float(144) / 255, float(255) / 255 };
const std::vector<vec2> mountain_centers = {};
const std::vector<vec2> sea_centers = { vec2(0.0f, 0.0f) };
const float mix_ratio = 0.2f;
const float sea_radius = 0.1f;
const float mountain_radius = 0.1f;

// Evaluate 3D position of the terrain for any (u,v) \in [-1,1]
vec3 evaluate_terrain(float x, float y)
{   
    const float z = corrected_radius + sqrt(pow(planet_radius, 2) - pow(x, 2) - pow(y, 2)) - corrected_radius;

    return {x,y,z};
}

mesh create_terrain()
{

    mesh terrain; // temporary terrain storage (CPU only)

    float x;
    float y;
    // Ajout des sommets
    for (int i = 0; i <= grid_steps; i++) {
        for (int j = 0; j <= grid_steps; j++) {
            x = -1.0f + 2 * float(i) / grid_steps + perturbation_shift_proportion / grid_steps * rand_interval();
            y = -1.0f + 2 * float(j) / grid_steps + perturbation_shift_proportion / grid_steps * rand_interval();
            terrain.position.push_back(evaluate_terrain(x + spacing, y + spacing));
            terrain.position.push_back(evaluate_terrain(x + spacing, y + spacing));
            terrain.position.push_back(evaluate_terrain(x + spacing, y - spacing));
            terrain.position.push_back(evaluate_terrain(x + spacing, y - spacing));
            terrain.position.push_back(evaluate_terrain(x - spacing, y - spacing));
            terrain.position.push_back(evaluate_terrain(x - spacing, y - spacing));
            terrain.position.push_back(evaluate_terrain(x - spacing, y + spacing));
            terrain.position.push_back(evaluate_terrain(x - spacing, y + spacing));
        }
    }

    // Ajout des triangles
    terrain.color.resize((grid_steps + 1) * (grid_steps + 1) * 8);
    for (unsigned int i = 0; i < grid_steps; i++) {
        for (unsigned int j = 0; j < grid_steps; j++) {
            bool choice = (rand_interval() > 0.5f);
            vec3 color_1 = generate_random_grey_color(vec2(-1.0f + 2 * float(i) / grid_steps, -1.0f + 2 * float(j) / grid_steps));
            vec3 color_2 = generate_random_grey_color(vec2(-1.0f + 2 * float(i) / grid_steps, -1.0f + 2 * float(j) / grid_steps));
            if (choice) {
                // Triangles lower-right, upper-left
                terrain.color[i * 8 * (grid_steps + 1) + 8 * j + 1] = color_1;
                terrain.color[(i + 1) * 8 * (grid_steps + 1) + 8 * j + 7] = color_1;
                terrain.color[(i + 1) * 8 * (grid_steps + 1) + 8 * (j + 1) + 4] = color_1;
                terrain.color[i * 8 * (grid_steps + 1) + 8 * j] = color_2;
                terrain.color[(i + 1) * 8 * (grid_steps + 1) + 8 * (j + 1) + 5] = color_2;
                terrain.color[i * 8 * (grid_steps + 1) + 8 * (j + 1) + 3] = color_2;
                // Triangle lower-right
                terrain.connectivity.push_back({ i * 8 * (grid_steps + 1) + 8 * j + 1, (i + 1) * 8 * (grid_steps + 1) + 8 * j + 7, (i + 1) * 8 * (grid_steps + 1) + 8 * (j + 1) + 4 });
                // Triangle upper-left
                terrain.connectivity.push_back({ i * 8 * (grid_steps + 1) + 8 * j, (i + 1) * 8 * (grid_steps + 1) + 8 * (j + 1) + 5, i * 8 * (grid_steps + 1) + 8 * (j + 1) + 3 });
            }
            else {
                // Triangles upper-right, lower-left
                terrain.color[i * 8 * (grid_steps + 1) + 8 * j] = color_1;
                terrain.color[(i + 1) * 8 * (grid_steps + 1) + 8 * j + 6] = color_1;
                terrain.color[i * 8 * (grid_steps + 1) + 8 * (j + 1) + 3] = color_1;
                terrain.color[(i + 1) * 8 * (grid_steps + 1) + 8 * j + 7] = color_2;
                terrain.color[(i + 1) * 8 * (grid_steps + 1) + 8 * (j + 1) + 4] = color_2;
                terrain.color[i * 8 * (grid_steps + 1) + 8 * (j + 1) + 2] = color_2;
                // Triangle lower-left
                terrain.connectivity.push_back({ i * 8 * (grid_steps + 1) + 8 * j, (i + 1) * 8 * (grid_steps + 1) + 8 * j + 6, i * 8 * (grid_steps + 1) + 8 * (j + 1) + 3});
                // Triangle upper-right
                terrain.connectivity.push_back({ (i + 1) * 8 * (grid_steps + 1) + 8 * j + 7, (i + 1) * 8 * (grid_steps + 1) + 8 * (j + 1) + 4, i * 8 * (grid_steps + 1) + 8 * (j + 1) + 2 });
            }
        }
    }

	terrain.fill_empty_field(); // need to call this function to fill the other buffer with default values (normal, color, etc)
    return terrain;
}

vec3 generate_random_grey_color(vec2 position)
{
    float distance;
    float min_distance_sea = 2.0f;
    float min_distance_mountain = 2.0f;
    for (int i = 0; i < sea_centers.size(); i++) {
        distance = norm(sea_centers[i] - position);
        if (distance < min_distance_sea) min_distance_sea = distance;
    }
    for (int i = 0; i < mountain_centers.size(); i++) {
        distance = norm(mountain_centers[i] - position);
        if (distance < min_distance_mountain) min_distance_mountain = distance;
    }
    // 1 corresponds to sea, 0 to mountain
    float mix_coefficient = 1 / (1 + exp(min_distance_mountain / mountain_radius - min_distance_sea / sea_radius)) * (1 - mix_ratio) + mix_ratio * (rand_interval() - 0.5f);
    const float rand_sea = rand_interval();
    const float rand_mountain = rand_interval();
    return mix_coefficient * (rand_mountain * color_grey_low + (1 - rand_mountain) * color_grey_high) + (1 - mix_coefficient) * (rand_sea * color_blue_low + (1 - rand_sea) * color_blue_high);
}

