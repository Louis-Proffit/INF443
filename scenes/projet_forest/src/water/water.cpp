#include "water.hpp"

#include "vcl/vcl.hpp"
#include <iostream>

using namespace vcl;

void Water::init_water()
{
    grid = mesh_primitive_grid(vec3(-10, -10, waterHeight), vec3(-10, 10, waterHeight), vec3(10, 10, waterHeight), vec3(10, -10, waterHeight), 100, 100);
}