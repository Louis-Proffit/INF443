#pragma once

#include "vcl/vcl.hpp"


vcl::vec3 evaluate_terrain(float x, float y);
vcl::mesh create_terrain();

vcl::vec3 generate_random_grey_color(vcl::vec2 position);


