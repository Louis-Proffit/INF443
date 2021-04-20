#include "tree_Lsystem/tree_LSys.hpp"
#include "vcl/vcl.hpp"
#include <iostream>
#include <string>

using namespace vcl;

mesh MeshGenerator::GenerateModel(std::string system, int iterations, std::string modelName, vec3 startingPoint, float radius)
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
                    cylindre = mesh_primitive_cylinder(norm(radiusVector), precPoint, startingPoint, 10, 20, true);
                    cylindre.color.fill({0.345f, 0.435f, 0.176f});
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
            startingPoint += translationVector;
            pointCollections.push_back(std::make_pair(startingPoint, radiusVector));
            if (norm(startingPoint - precPoint) != 0)
            {
                mesh cylindre;
                cylindre = mesh_primitive_cylinder(norm(radiusVector), precPoint, startingPoint, 10, 20, true);
                cylindre.color.fill({0.345f, 0.435f, 0.176f});
                result.push_back(cylindre);
            }
            //std::cout << startingPoint << std::endl;
            //std::cout << " " << std::endl;
            //feuille = mesh_primitive_sphere(0.01f, startingPoint, 40, 20);
            feuille = GenerateLeave(startingPoint, translationVector, 0.10f);
            feuille.color.fill({1.0f, 0, 0});
            result.push_back(feuille);
            break;
        default:
            break;
        }
    }

    result.fill_empty_field();
    return result;
}

mesh MeshGenerator::GenerateLeave(vec3 startingPoint, vec3 translationVector, float height)
{
    // mesh feuille1 = mesh_primitive_triangle(startingPoint, startingPoint + vec3(0.02f, 0, 0), startingPoint + vec3(0, 0, 0.04f));
    //mesh feuille2 = mesh_primitive_triangle(startingPoint, startingPoint + vec3(0.02f, 0, 0), startingPoint + vec3(0, 0, 0.04f));
    //mesh feuille3 = mesh_primitive_triangle(startingPoint, startingPoint + vec3(0.02f, 0, 0), startingPoint + vec3(0, 0, 0.04f));
    int rotat = PI / 8;
    rotation rotationX = rotation({1, 0, 0}, rotat);
    rotation negRotationX = rotation({-1, 0, 0}, rotat);
    rotation rotationY = rotation({0, 1, 0}, rotat);
    rotation negRotationY = rotation({0, -1, 0}, rotat);
    rotation rotationZ = rotation({0, 0, 1}, rotat);
    rotation negRotationZ = rotation({0, 0, -1}, rotat);
    mesh feuille1 = mesh_primitive_triangle(startingPoint - vec3(height / 4, 0, 0), startingPoint + vec3(height / 4, 0, 0), startingPoint + height * normalize(rotationX * negRotationY * translationVector));
    mesh feuille2 = mesh_primitive_triangle(startingPoint - vec3(height / 4, 0, 0), startingPoint + vec3(0, height / 4, 0), startingPoint + height * normalize(rotationZ * rotationY * translationVector));
    mesh feuille3 = mesh_primitive_triangle(startingPoint - vec3(0, height / 4, 0), startingPoint + vec3(height / 4, 0, 0), startingPoint + height * normalize(negRotationX * negRotationZ * translationVector));
    feuille1.push_back(feuille2);
    feuille1.push_back(feuille3);
    return feuille1;
}