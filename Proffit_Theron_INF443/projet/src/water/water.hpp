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
    GLuint dudvmap = 0;
    GLuint normalMap = 0;
    GLuint shader = 0;

    void init_water(GLuint shad, float x_min = -10.0, float x_max = 10.0, float y_min = -10.0, float y_max = 10.0);

    void set_Uniforms(GLuint reflectiontexture, GLuint refractiontexture, vec3 campos, float movefactor);

    template <typename SCENE>
    void draw_water(SCENE const &scene)
    {
        draw(waterd, scene);
    }
};