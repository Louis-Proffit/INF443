#pragma once

#include "vcl/vcl.hpp"

vcl::vec3 evaluate_terrain(float u, float v);
vcl::mesh create_terrain();
std::vector<vcl::vec3> generate_positions_on_terrain(int N);

struct perlin_noise_parameters
{
    float persistency = 0.45f;
    float frequency_gain = 2.35f;
    int octave = 6;
    float terrain_height = 1.0f;
};

// Recompute the vertices of the terrain everytime a parameter is modified
//  and update the mesh_drawable accordingly
void update_terrain(std::vector<std::vector<float>> &heightData, vcl::mesh &terrain, vcl::mesh_drawable &terrain_visual, perlin_noise_parameters const &parameters);
