#include "tree_LSys.hpp"
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

    for (int i = 0; i < system.size(); i++)
    {
        switch (system[i])
        {
        case 't': //translate
            startingPoint -= translationVector;
            break;
        case 'T':
            startingPoint += translationVector;
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
        }
        break;
        case 'c':
        case 'C':
            closeOffIndeces.push_back(pointCollections.size() - 1);
            break;
        case '+': //pushes the current point to be used on the geometry
            pointCollections.push_back(std::make_pair(startingPoint, radiusVector));
            break;
        case 'F':
            startingPoint += translationVector;
            pointCollections.push_back(std::make_pair(startingPoint, radiusVector));
            break;
        default:
            break;
        }
    }
    int longueur = pointCollections.size();
    for (int i = 0; i < longueur - 1; i++)
    {
        vec3 vertex_sommet;
        vertex_sommet = pointCollections[i + 1].first;
        vec3 radVec = pointCollections[i].second;
        vec3 base = pointCollections[i].first;
        if ((closeOffIndeces.size() > 0 && i == closeOffIndeces.front()))
        {
            mesh cone;
            cone = mesh_primitive_cone(norm(radVec), norm(radVec), base, {0, 0, 1});
            closeOffIndeces.erase(closeOffIndeces.begin());
            result.push_back(cone);
        }
        else
        {
            mesh cylindre;
            cylindre = mesh_primitive_cylinder(norm(radVec), base, vertex_sommet, 10, 20, true);

            result.push_back(cylindre);
        }
        result.color.fill({0.345f, 0.435f, 0.176f});
    }

    /* int longueur = pointCollections.size();
    result.position.resize(longueur * pointsPerLevel + longueur);
    for (int i = 0; i < longueur; i++)
    {
        for (int n = 0; n < pointsPerLevel; n++)
        {
            vec3 vertex;
            vertex = pointCollections[i].first;
            vec3 radVec = pointCollections[i].second;
            vec3 rotAxis = vertex;
            if (i - 1 > 0)
                rotAxis = rotAxis - pointCollections[i - 1].first;
            if ((norm(vertex)) > 0)
            {
                if (norm(rotAxis) > 0)
                    normalize(rotAxis);
            }
            else if (i + 1 < pointCollections.size())
                rotAxis = pointCollections[i + 1].first - rotAxis;
            else //Assume 0,1,0
                rotAxis = vec3(0, 1, 0);
            float theta = 2 * PI / pointsPerLevel * n;
            radVec = radVec * cos(theta) + (cross(rotAxis, radVec)) * sin(theta) + rotAxis * (dot(rotAxis, radVec)) * (1 - cos(theta));
            result.position[n + i * pointsPerLevel] = vertex + radVec;
        }
        result.position[longueur * pointsPerLevel + i] = pointCollections[i].first;
    }

    //Create the face geometry for the mesh
    /* for (int i = 0; i < meshGeometry.size(); i++)
    {
        int currentSize = meshGeometry[i].size();
        int nextRowSize = meshGeometry[(i + 1) % meshGeometry.size()].size();
        bool closedOff = false;
        for (int k = 0; k < currentSize; k++)
        {
            //Draw to the next layer of points, unless this is the first or last layer.
            //In those situations the shape needs to close, so connect those to the center points.
            if (i == 0)
            {
                m_model->AddTri(meshGeometry[i][k], meshGeometry[i][(k + 1) % currentSize], pointCollections[i].first);
            }
            if (i == meshGeometry.size() - 1)
            {
                m_model->AddTri(meshGeometry[i][k], meshGeometry[i][(k + 1) % currentSize], pointCollections[i].first);
                continue; //There are no more points to draw up to
            }
            //Don't connect points to the next layer of the index is a close off point
            if (closedOff || (closeOffIndeces.size() > 0 && i == closeOffIndeces.front()))
            {
                m_model->AddTri(meshGeometry[i][k], meshGeometry[i][(k + 1) % currentSize], pointCollections[i].first);
                closedOff = true;
                continue;
            }
            //Need to draw the face
            // __
            //|\ |
            //|_\|
            //Two triangles per face
            //Draw the first triangle of the face
            m_model->AddTri(meshGeometry[i][k], meshGeometry[i][(k + 1) % currentSize], meshGeometry[i + 1][k]);
            m_model->AddTri(meshGeometry[i + 1][k], meshGeometry[i + 1][(k + 1) % currentSize], meshGeometry[i][(k + 1) % currentSize]);
        }
        if (closedOff)
            closeOffIndeces.erase(closeOffIndeces.begin());
    }
    */
    /* for (int i = 0; i < longueur; i++)
    {
        int currentSize = pointsPerLevel;
        bool closedOff = false;
        for (int k = 0; k < currentSize; k++)
        {
            const unsigned int idx = k + pointsPerLevel * i;
            //Draw to the next layer of points, unless this is the first or last layer.
            //In those situations the shape needs to close, so connect those to the center points.
            int j = (k + 1) % currentSize;
            if (i == 0)
            {
                const uint3 triangle_1 = {idx, j + pointsPerLevel * i, longueur * pointsPerLevel + i};
                result.connectivity.push_back(triangle_1);
            }
            if (i == longueur - 1)
            {
                const uint3 triangle_1 = {idx, j + pointsPerLevel * i, longueur * pointsPerLevel + i};
                result.connectivity.push_back(triangle_1);
                continue; //There are no more points to draw up to
            }
            //Don't connect points to the next layer of the index is a close off point
            if (closedOff || (closeOffIndeces.size() > 0 && i == closeOffIndeces.front()))
            {
                const uint3 triangle_1 = {idx, j + pointsPerLevel * i, longueur * pointsPerLevel + i};
                result.connectivity.push_back(triangle_1);
                closedOff = true;
                continue;
            }
            //Need to draw the face
            // __
            //|\ |
            //|_\|
            //Two triangles per face
            //Draw the first triangle of the face
            const uint3 triangle_1 = {idx, j + pointsPerLevel * i, idx + pointsPerLevel};
            const uint3 triangle_2 = {idx + pointsPerLevel, j + pointsPerLevel * (i + 1), j + pointsPerLevel * i};

            result.connectivity.push_back(triangle_1);
            result.connectivity.push_back(triangle_2);
        }
        if (closedOff)
            closeOffIndeces.erase(closeOffIndeces.begin());
    }*/
    result.fill_empty_field();
    return result;
}