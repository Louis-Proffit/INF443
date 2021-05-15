
#include "terrain.hpp"

using namespace vcl;

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v)
{
    float const x = 20 * (u - 0.5f);
    float const y = 20 * (v - 0.5f);

    // vec2 const u0 = {0.5f, 0.5f};
    //float const h0 = 2.0f;
    //float const sigma0 = 0.15f;
    float H[4] = {3.0f, -1.5f, 1.0f, 2.0f};
    vec2 P[4] = {{-0.7f, 0.5f}, {0.5f, 0.5f}, {0.2f, 0.7f}, {0.8f, 0.7f}};
    float sigm[4] = {0.5f, 0.15f, 0.2f, 0.2f};

    //float const d = norm(vec2(u, v) - u0) / sigma0;

    float z = 0;
    float d;
    for (int i = 0; i < 4; i++)
    {
        d = norm(vec2(u, v) - P[i]) / sigm[i];
        z += H[i] * std::exp(-d * d);
    }

    return {x, y, z};
}

mesh create_terrain()
{
    // Number of samples of the terrain is N x N
    const unsigned int N = 100;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N * N);
    terrain.uv.resize(N * N);

    // Fill terrain geometry
    for (unsigned int ku = 0; ku < N; ++ku)
    {
        for (unsigned int kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            // Compute the local surface function
            vec3 const p = evaluate_terrain(u, v);

            // Store vertex coordinates
            terrain.position[kv + N * ku] = p;
            terrain.uv[kv + N * ku] = {p.x, p.y};
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for (size_t ku = 0; ku < N - 1; ++ku)
    {
        for (size_t kv = 0; kv < N - 1; ++kv)
        {
            const unsigned int idx = kv + N * ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx + 1 + N, idx + 1};
            const uint3 triangle_2 = {idx, idx + N, idx + 1 + N};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    terrain.fill_empty_field(); // need to call this function to fill the other buffer with default values (normal, color, etc)
    return terrain;
}

std::vector<vcl::vec3> generate_positions_on_terrain(int N)
{
    std::vector<vcl::vec3> res;
    for (int i = 0; i < N; i++)
    {
        float u = rand_interval();
        float v = rand_interval();
        vec3 p = evaluate_terrain(u, v);
        bool enlv = false;
        for (vec3 q : res)
        {
            if (((q.x - p.x) * (q.x - p.x) + (q.y - p.y) * (q.y - p.y)) < 0.10) // Passer environ à 0.20 pour eviter les collisions entre les tree
            {
                enlv = true;
            }
        }
        if (!enlv)
        {
            res.push_back(p);
        }
    }
    return res;
}

void update_terrain(std::vector<std::vector<float>> &heightData, mesh &terrain, mesh_drawable &terrain_visual, perlin_noise_parameters const &parameters)
{
    // Number of samples in each direction (assuming a square grid)
    int const N = std::sqrt(terrain.position.size());

    // Recompute the new vertices
    for (int ku = 0; ku < N; ++ku)
    {
        for (int kv = 0; kv < N; ++kv)
        {

            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            int const idx = ku * N + kv;

            // Compute the Perlin noise
            float const noise = noise_perlin({u, v}, parameters.octave, parameters.persistency, parameters.frequency_gain);

            // use the noise as height value
            terrain.position[idx].z = (noise) + heightData[u][v];

            // use also the noise as color value
            terrain.color[idx] = 0.3f * vec3(0, 0.5f, 0) + 0.7f * noise * vec3(1, 1, 1);
        }
    }

    // Update the normal of the mesh structure
    terrain.compute_normal();

    // Update step: Allows to update a mesh_drawable without creating a new one
    terrain_visual.update_position(terrain.position);
    terrain_visual.update_normal(terrain.normal);
    terrain_visual.update_color(terrain.color);
}