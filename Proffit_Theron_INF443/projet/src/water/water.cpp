#include "water.hpp"

#include "vcl/vcl.hpp"
#include <iostream>
#include "helpers/scene_helper.hpp"

using namespace vcl;

void Water::init_water(GLuint shad, float x_min, float x_max, float y_min, float y_max)
{
    grid = mesh_primitive_grid(vec3(x_min, y_min, waterHeight), vec3(x_min, y_max, waterHeight), vec3(x_max, y_max, waterHeight), vec3(x_max, y_min, waterHeight), 100, 100);
    shader = shad;
    dudvmap = scene_visual::get_texture(texture_type::WATERDUDV);
    normalMap = scene_visual::get_texture(texture_type::WATERNORMAL);
    waterd = mesh_drawable(grid, shader);
}
void Water::set_Uniforms(GLuint reflectiontexture, GLuint refractiontexture, vec3 campos, float movefactor)
{

    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE3);
    opengl_check;
    glBindTexture(GL_TEXTURE_2D, reflectiontexture);
    opengl_check;
    opengl_uniform(shader, "reflection_texture", 3);
    opengl_check;
    glActiveTexture(GL_TEXTURE4);
    opengl_check;
    glBindTexture(GL_TEXTURE_2D, refractiontexture);
    opengl_check;
    opengl_uniform(shader, "refraction_texture", 4);
    opengl_check;
    glActiveTexture(GL_TEXTURE5);
    opengl_check;
    glBindTexture(GL_TEXTURE_2D, dudvmap);
    opengl_check;
    opengl_uniform(shader, "dudvmap", 5);
    opengl_check;
    opengl_uniform(shader, "movefactor", movefactor);
    opengl_check;
    opengl_uniform(shader, "cameraPosition", campos);
    glActiveTexture(GL_TEXTURE6);
    opengl_check;
    glBindTexture(GL_TEXTURE_2D, normalMap);
    opengl_check;
    opengl_uniform(shader, "normalMap", 6);
    opengl_check;
}