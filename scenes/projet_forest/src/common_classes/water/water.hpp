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

    void init_water(GLuint shad);

    void set_Uniforms(GLuint reflectiontexture, GLuint refractiontexture, vec3 campos, float movefactor);

    template <typename SCENE>
    void draw_water(SCENE const &scene)
    {
        vec3 light = scene.camera.position();
        glUseProgram(shader);
        opengl_uniform(shader, "projection", scene.projection);
        opengl_uniform(shader, "view", scene.camera.matrix_view());
        opengl_uniform(shader, "light", light);
        draw(waterd, scene);
    }
};