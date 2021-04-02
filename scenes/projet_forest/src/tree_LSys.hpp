#pragma once
#include "LSystem.hpp"
#include "vcl/vcl.hpp"
#include <iostream>
//#include "Model.hpp"

using namespace vcl;

#define PI 3.14159265359f

class MeshGenerator
{
public:
    LSytem m_system;
    float rotationOffset = PI / 8;
    float scaleOffset = 1.5f;
    float translationOffset = 1.0f;
    mesh GenerateModel(std::string system, int iterations, std::string modelName, vec3 startingPoint, float radius, int pointsPerLevel);

private:
};