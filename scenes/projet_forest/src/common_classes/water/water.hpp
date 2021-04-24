#pragma once

#include "vcl/vcl.hpp"
#include <iostream>

using namespace vcl;

class Water
{
public:
    float waterHeight = 0;
    mesh grid;
    mesh_drawable waterd;
    GLuint dudvmap = opengl_texture_to_gpu(image_load_png("../../assets/water/waterdudv.png"), GL_REPEAT, GL_REPEAT);
    GLuint normalMap = opengl_texture_to_gpu(image_load_png("../../assets/water/waternormal.png"), GL_REPEAT, GL_REPEAT);
    GLuint shader;

    void init_water(GLuint shad);
};