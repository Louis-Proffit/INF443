#include "tree_LSys.hpp"
#include "vcl/vcl.hpp"
#include <iostream>
#include <string>
#include <random>

using namespace vcl;

void TreeGenerator::GenerateModel(std::string system, int iterations, std::string modelName, vec3 startingPoint, float radius, GLuint shader)
{
    mesh result;
    system = m_system.ApplyAxioms(system, iterations);
    std::vector<std::pair<vec3, vec3>> pointCollections;
    vec3 radiusVector = vec3(radius, 0, 0);
    std::vector<std::pair<vec3, std::pair<vec3, vec3>>> pointStack;
    std::vector<int> closeOffIndeces;
    vec3 translationVector = vec3(0, 0, translationOffset);
    rotation rotationX = rotation({1, 0, 0}, rotationOffset);
    rotation negRotationX = rotation({-1, 0, 0}, rotationOffset);
    rotation rotationY = rotation({0, 1, 0}, rotationOffset);
    rotation negRotationY = rotation({0, -1, 0}, rotationOffset);
    rotation rotationZ = rotation({0, 0, 1}, rotationOffset);
    rotation negRotationZ = rotation({0, 0, -1}, rotationOffset);
    mesh cylindre;
    mesh feuille;
    vec3 endingpoint;

    // std::cout << system << std::endl;
    pointCollections.push_back(std::make_pair(startingPoint, radiusVector));
    vec3 precPoint;
    precPoint = startingPoint;
    bool relie = true;

    for (int i = 0; i < system.size(); i++)
    {
        /*std::cout << startingPoint << std::endl;
        std::cout << translationVector << std::endl;
        std::cout << " " << std::endl;*/
        switch (system[i])
        {
        case 't': //translate
            startingPoint -= translationVector;
            break;
        case 'T':
            startingPoint += translationVector;
            break;
        case 'p':
            std::cout << translationVector << std::endl;
            translationVector = translationVector / scaleOffset;
            std::cout << translationVector << std::endl;
            break;
        case 'P':
            translationVector *= scaleOffset;
            break;
        case 's': //scale smaller
            radiusVector /= scaleOffset;
            break;
        case 'S': //scale larger
            radiusVector *= scaleOffset;
            break;
        case 'x':
            translationVector = rotationX * translationVector;
            break;
        case 'X':
            translationVector = negRotationX * translationVector;
            break;
        case 'y':
            translationVector = rotationY * translationVector;
            break;
        case 'Y':
            translationVector = negRotationY * translationVector;
            break;
        case 'z':
            translationVector = rotationZ * translationVector;
            break;
        case 'Z':
            translationVector = negRotationZ * translationVector;
            break;
        case '[': //push point onto the stack
            if (pointCollections.size() > 0)
            {
                std::pair<vec3, vec3> pointCollectionBack = pointCollections.back();
                pointStack.push_back(std::make_pair(pointCollectionBack.first, std::make_pair(pointCollectionBack.second, translationVector)));
            }
            else
                pointStack.push_back(make_pair(startingPoint, std::make_pair(radiusVector, translationVector)));
            break;
        case ']':
        { //pop point from the stack
            std::pair<vec3, std::pair<vec3, vec3>> popped = pointStack.back();
            pointStack.pop_back();
            startingPoint = popped.first;
            radiusVector = popped.second.first;
            translationVector = popped.second.second;
            //relie = false;
            pointCollections.push_back(std::make_pair(startingPoint, radiusVector));
            precPoint = startingPoint;
        }
        break;
        case 'c':
        case 'C':
            closeOffIndeces.push_back(pointCollections.size() - 1);
            pointCollections.push_back(std::make_pair(startingPoint, radiusVector));
            precPoint = startingPoint;
            break;
        case '+': //pushes the current point to be used on the geometry
            pointCollections.push_back(std::make_pair(startingPoint, radiusVector));
            break;
        case 'F':
            startingPoint += translationVector;
            pointCollections.push_back(std::make_pair(startingPoint, radiusVector));
            if (relie)
            {
                if (norm(startingPoint - precPoint) != 0)
                {
                    mesh cylindre;
                    cylindre = mesh_primitive_cylinder(norm(radiusVector), precPoint, startingPoint, 3, 6, true);
                    cylindre.color.fill({0.451f, 0.251f, 0.071f});
                    result.push_back(cylindre);
                }
            }
            else
            {
                relie = true;
            }
            precPoint = startingPoint;
            break;
        case 'H':
            hasleaves = true;
            startingPoint += translationVector;
            pointCollections.push_back(std::make_pair(startingPoint, radiusVector));
            if (norm(startingPoint - precPoint) != 0)
            {
                mesh cylindre;
                cylindre = mesh_primitive_cylinder(norm(radiusVector), precPoint, startingPoint, 3, 6, true);
                cylindre.color.fill({0.451f, 0.251f, 0.071f});
                result.push_back(cylindre);
            }
            //std::cout << startingPoint << std::endl;
            //std::cout << " " << std::endl;
            //feuille = mesh_primitive_sphere(0.01f, startingPoint, 40, 20);
            GenerateLeave(startingPoint, translationVector, 0.03f);
            //feuille.color.fill({1.0f, 0, 0});
            //result.push_back(feuille);
            break;
        default:
            break;
        }
    }

    result.fill_empty_field();
    trunk = result;
    dtrunk = mesh_drawable(trunk, shader);
    if (hasleaves)
    {
        leaves.fill_empty_field();
        dleaves = mesh_drawable(leaves, shader);
    }
}

void TreeGenerator::GenerateLeave(vec3 startingPoint, vec3 translationVector, float height)
{

    affine_rts trans;
    //trans.scale = height;
    trans.translate = startingPoint;
    // std::cout << rotation_between_vector(vec3(0, 0, 1), translationVector) << std::endl;
    trans.rotate = rotation_between_vector(vec3(0, 0, 1), normalize(translationVector));
    mesh feuille = mesh_primitive_quadrangle(trans * vec3(-height, -height, 0), trans * vec3(height, -height, 0), trans * vec3(height, height, 0), trans * vec3(-height, height, 0));
    for (auto i = 0; i < feuille.uv.size(); i++)
        leaves.push_back(feuille);
}

void TreeGenerator::initTree(tree_type tree_type, GLuint shader, bool blockleaves)
{
    switch (tree_type)
    {
    case tree_type::REAL_1:
        m_system.ClearAxioms();
        translationOffset = .1f;
        scaleOffset = 1.35f;
        rotationOffset = 3.14f / 4;
        m_system.AddAxiom('H', "F[[[[[[xH]XH]zyxH]ZYXH]yXH]YxH]H");
        m_system.AddAxiom('F', "FsF");
        GenerateModel("H", 4, "Test", vec3(0, 0, 0), .05f, shader);
        dtrunk.transform.scale = 3.0f;
        dleaves.transform.scale = 3.0f;
        dtrunk.texture = opengl_texture_to_gpu(image_load_png("../assets/textures/tree/trunk.png"));
        dleaves.texture = opengl_texture_to_gpu(image_load_png("../assets/textures/tree/pine.png"));
        break;
    case tree_type::REAL_2:
        std::cout << "Generating RealTree_2" << std::endl;
        m_system.ClearAxioms();
        translationOffset = .1f;
        scaleOffset = 1.05f;
        rotationOffset = 3.14f / 3;
        m_system.AddAxiom('H', "F[[[[xH]XH]yH]YH]tFH");
        m_system.AddAxiom('F', "FF");
        GenerateModel("H", 4, "Test", vec3(0, 0, 0), .01f, shader);
        dtrunk.texture = opengl_texture_to_gpu(image_load_png("../assets/textures/tree/trunk.png"));
        dleaves.texture = opengl_texture_to_gpu(image_load_png("../assets/textures/tree/fern2.png"));
        break;
    case tree_type::REAL_3:
        std::cout << "Generating RealTree3" << std::endl;
        m_system.ClearAxioms();
        translationOffset = .1f;
        scaleOffset = 1.5f;
        rotationOffset = 3.14f / 4;
        m_system.AddAxiom('H', "F[[[[[[xH]XH]zyxH]ZYXH]yXH]YxH]tFH");
        m_system.AddAxiom('F', "FF");
        GenerateModel("H", 4, "Test", vec3(0, 0, 0), .01f, shader);
        break;
    case tree_type::CLASSIC:
        std::cout << "Generating Classique" << std::endl;
        m_system.ClearAxioms();
        translationOffset = .1f;
        scaleOffset = 1.8f;
        rotationOffset = 3.14f / 4;
        m_system.AddAxiom('H', "F[xsH][XsH][ysH][YsH]");
        GenerateModel("FFH", 5, "Test", vec3(0, 0, 0), .03f, shader);
        dtrunk.transform.scale = 3.0f;
        dleaves.transform.scale = 3.0f;
        dtrunk.texture = opengl_texture_to_gpu(image_load_png("../assets/textures/tree/trunk.png"));
        dleaves.texture = opengl_texture_to_gpu(image_load_png("../assets/textures/tree/pine.png"));
        if (blockleaves)
            hasleaves = false;
        break;
    case tree_type::COOL:
        std::cout << "Generating coolTree..." << std::endl;
        m_system.ClearAxioms();
        rotationOffset = 0.4485496f;
        translationOffset = 0.1f;
        scaleOffset = 1.0f;
        m_system.AddAxiom('R', "F[ZxR][ZyR][ZzR][YxR][YyR][YzR][XxR][XyR][XzR][xR][yR][zR]FR");
        m_system.AddAxiom('F', "FF");
        dtrunk.texture = opengl_texture_to_gpu(image_load_png("../assets/textures/tree/trunk.png"));
        dleaves.texture = opengl_texture_to_gpu(image_load_png("../assets/textures/tree/pine.png"));
        GenerateModel("R", 4, "Tree2", vec3(0, 0, 0), .01f, shader);
        break;
    case tree_type::SAPIN:
        std::cout << "Generating Sapin" << std::endl;

        m_system.ClearAxioms();
        translationOffset = .1f;
        scaleOffset = 1.35f;
        rotationOffset = 2 * 3.14f / 3;
        m_system.AddAxiom('H', "F[xH][XH][yH][YH]H");
        m_system.AddAxiom('F', "FF");
        GenerateModel("H", 5, "Test", vec3(0, 0, 0), .01f, shader);
        dtrunk.transform.scale = 3.0f;
        dleaves.transform.scale = 3.0f;
        dtrunk.texture = opengl_texture_to_gpu(image_load_png("../../assets/textures/trunk.png"));
        dleaves.texture = opengl_texture_to_gpu(image_load_png("../../assets/textures/leaf.png"));
        if (blockleaves)
            hasleaves = false;
        break;
    case tree_type::BUISSON:
        std::cout << "Generating buisson..." << std::endl;
        m_system.ClearAxioms();
        rotationOffset = .4f;
        translationOffset = .2f;
        scaleOffset = 1.0f;
        m_system.AddAxiom('R', "YYTF[xFR]C[XFRFR]");
        GenerateModel("+TT+R", 7, "Tree5", vec3(0, 0, 0), .02f, shader);
        break;
    case tree_type::FOUGERE:
        m_system.ClearAxioms();
        rotationOffset = 0.3926991f;
        translationOffset = 0.1f;
        m_system.AddAxiom('F', "F[Fx[[zFZXFZYF]X[ZFxzFyzF]][ZFzYFzXF]Y[zFyZFxZF]C+]");
        GenerateModel("+TT+F", 3, "Fougere", vec3(0, 0, 0), 0.01f, shader);
        break;
    case tree_type::BRIN:
        std::cout << "Generating brin ..." << std::endl;
        m_system.ClearAxioms();
        rotationOffset = 0.3f;
        translationOffset = 0.4f;
        m_system.AddAxiom('R', "FFF[FXYZ[FxRxF[zFRzXFC]R[ZFZyFC]]yFRyF]");
        GenerateModel("+TT+R", 5, "Brin", vec3(0, 0, 0), .1f, shader);
        break;
    case tree_type::WTF:
        std::cout << "Generating wtf..." << std::endl;
        m_system.ClearAxioms();
        rotationOffset = 0.1f;
        translationOffset = 0.1f;
        scaleOffset = 1.0f;
        m_system.AddAxiom('R', "F[[yyBBzB]XB]");
        m_system.AddAxiom('B', "XXYYYYYYYYFRFzzFRRC");
        GenerateModel("+TT+R", 7, "Tree4", vec3(0, 0, 0), .01f, shader);
        break;
    }
}

tree_type TreeGenerator::random_tree_type()
{
    float random = rand_interval();
    if (random < 0.33)
        return tree_type::CLASSIC;
    else if (random < 0.66)
        return tree_type::COOL;
    if (random < 1.0)
        return tree_type::REAL_1;
}

void TreeGenerator::translate(vec3 _translation)
{
    dtrunk.transform.translate = _translation;
    dleaves.transform.translate = _translation;
}

void TreeGenerator::resize(float taille)
{
    dtrunk.transform.scale = taille;
    dleaves.transform.scale = taille;
}