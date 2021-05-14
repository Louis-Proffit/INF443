#pragma once
#include "common_classes/tree_Lsystem/LSystem.hpp"
#include "vcl/vcl.hpp"
#include <iostream>
//#include "Model.hpp"

using namespace vcl;

#define PI 3.14159265359f

class TreeGenerator
{
public:
    LSytem m_system;
    float rotationOffset = PI / 8;
    float scaleOffset = 5.0f;
    float translationOffset = 1.0f;
    mesh trunk;
    mesh leaves;
    mesh_drawable dtrunk;
    mesh_drawable dleaves;
    bool hasleaves = false;

    void GenerateModel(std::string system, int iterations, std::string modelName, vec3 startingPoint, float radius, GLuint shader = shader_basic_w);

    void GenerateLeave(vec3 startingPoint, vec3 translationVector, float height);

    void translate(vec3 _translation);

    void initTree(std::string treename = "brin", bool blockleaves = false, GLuint shader = shader_basic_w);

    template <typename SCENE>
    void draw_tree(SCENE const &scene)
    {
        vec3 light = scene.camera.position();
        glUseProgram(dtrunk.shader);
        opengl_uniform(dtrunk.shader, "projection", scene.projection);
        opengl_uniform(dtrunk.shader, "view", scene.camera.matrix_view());
        opengl_uniform(dtrunk.shader, "light", light);

        draw(dtrunk, scene);

        if (hasleaves)
        {
            glUseProgram(dleaves.shader);
            opengl_uniform(dleaves.shader, "projection", scene.projection);
            opengl_uniform(dleaves.shader, "view", scene.camera.matrix_view());
            opengl_uniform(dleaves.shader, "light", light);
            draw(dleaves, scene);
        }
    }

    static GLuint shader_basic_w;
};