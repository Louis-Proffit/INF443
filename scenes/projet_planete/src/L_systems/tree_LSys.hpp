#pragma once
#include "L_systems/LSystem.hpp"
#include "vcl/vcl.hpp"
#include <iostream>

#define PI 3.14159265359f

enum class tree_type
{
    CLASSIC,
    REAL_1,
    REAL_2,
    REAL_3,
    COOL,
    SAPIN,
    FOUGERE,
    BRIN,
    WTF,
    BUISSON
};

struct tree_located
{
    tree_type type;
    vcl::vec3 position;
};

class TreeGenerator
{
public:
    LSytem m_system;
    float rotationOffset = PI / 8;
    float scaleOffset = 5.0f;
    float translationOffset = 1.0f;
    vcl::mesh trunk;
    vcl::mesh leaves;
    vcl::mesh_drawable dtrunk;
    vcl::mesh_drawable dleaves;
    bool hasleaves = false;

    void GenerateModel(std::string system, int iterations, std::string modelName, vcl::vec3 startingPoint, float radius, GLuint shader);

    void GenerateLeave(vcl::vec3 startingPoint, vcl::vec3 translationVector, float height);

    void translate(vcl::vec3 _translation);

    void resize(float taille);

    void initTree(tree_type treename, GLuint shader, bool blockleaves = false);

    static tree_type random_tree_type();
};