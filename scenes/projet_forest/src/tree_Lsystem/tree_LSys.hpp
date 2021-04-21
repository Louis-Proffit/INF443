#pragma once
#include "tree_Lsystem/LSystem.hpp"
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

    void GenerateModel(std::string system, int iterations, std::string modelName, vec3 startingPoint, float radius);

    mesh GenerateLeave(vec3 startingPoint, vec3 translationVector, float height);

    void initTree(std::string treename = "brin");

    template <typename SCENE>
    void draw_tree(SCENE const &scene)
    {
        draw(dtrunk, scene);

        if (hasleaves)
            draw(dleaves, scene);
    }
};