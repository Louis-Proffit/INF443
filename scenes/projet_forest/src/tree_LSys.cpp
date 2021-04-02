#include "tree_LSys.hpp"
#include "vcl/vcl.hpp"
#include <iostream>
#include <string>

using namespace vcl;

mesh MeshGenerator::GenerateModel(std::string system, int iterations, std::string modelName, vec3 startingPoint, float radius, int pointsPerLevel)
{
    mesh result;
    system = m_system.ApplyAxioms(system, iterations);
    std::vector<std::pair<vec3, vec3>> pointCollections;
    vec3 radiusVector = vec3(radius, 0, 0);
    std::vector<std::pair<vec3, std::pair<vec3, vec3>>> pointStack;
    std::vector<int> closeOffIndeces;
    vec3 translationVector = vec3(0, translationOffset, 0);
    /*quaternion rotationX = quat(vec3(rotationOffset, 0, 0));
    quat negRotationX = quat(vec3(-rotationOffset, 0, 0));
    quat rotationY = quat(vec3(0, rotationOffset, 0));
    quat negRotationY = quat(vec3(0, -rotationOffset, 0));
    quat rotationZ = quat(vec3(0, 0, rotationOffset));
    quat negRotationZ = quat(vec3(0, 0, -rotationOffset));*/

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
        /*case 'x':
            translationVector = rotate(rotationX, translationVector);
            break;
        case 'X':
            translationVector = rotate(negRotationX, translationVector);
            break;
        case 'y':
            translationVector = rotate(rotationY, translationVector);
            break;
        case 'Y':
            translationVector = rotate(negRotationY, translationVector);
            break;
        case 'z':
            translationVector = rotate(rotationZ, translationVector);
            break;
        case 'Z':
            translationVector = rotate(negRotationZ, translationVector);
            break;*/
        case '[': //push point onto the stack
            if (pointCollections.size() > 0)
            {
                std::pair<vec3, vec3> pointCollectionBack = pointCollections.back();
                pointStack.push_back(make_pair(pointCollectionBack.first, std::make_pair(pointCollectionBack.second, translationVector)));
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
    //Create the vertices for the mesh
    /*vector<vector<vec3>> meshGeometry;
    for (int i = 0; i < pointCollections.size(); i++)
    {
        vector<vec3> pointsForIteration;
        for (int n = 0; n < pointsPerLevel; n++)
        {
            vec3 vertex;
            vertex = pointCollections[i].first;
            vec3 radVec = pointCollections[i].second;
            vec3 rotAxis = vertex;
            if (i - 1 > 0)
                rotAxis = rotAxis - pointCollections[i - 1].first;
            if (length(vertex) > 0)
                rotAxis = normalize(rotAxis);
            else if (i + 1 < pointCollections.size())
                rotAxis = pointCollections[i + 1].first - rotAxis;
            else //Assume 0,1,0
                rotAxis = vec3(0, 1, 0);
            float theta = 2 * PI / pointsPerLevel * n;
            radVec = radVec * cos(theta) + (cross(rotAxis, radVec)) * sin(theta) + rotAxis * (dot(rotAxis, radVec)) * (1 - cos(theta));
            pointsForIteration.push_back(vertex + radVec);
        }
        meshGeometry.push_back(pointsForIteration);
    }*/
    int longueur = pointCollections.size();
    result.position.resize(longueur * pointsPerLevel);
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
            if ((vertex.x * vertex.x + vertex.y * vertex.y + vertex.z + vertex.z) > 0)
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
            result.position[n + i * pointsPerLevel];
        }
    }

    //Create the face geometry for the mesh
    /*for (int i = 0; i < meshGeometry.size(); i++)
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
    }*/
    for (int i = 0; i < longueur - 1; i++)
    {
        int currentSize = pointsPerLevel;
        int nextRowSize = pointsPerLevel;
        bool closedOff = false;
        for (int k = 0; k < pointsPerLevel - 1; k++)
        {
            const unsigned int idx = k + pointsPerLevel * i;
            //Draw to the next layer of points, unless this is the first or last layer.
            //In those situations the shape needs to close, so connect those to the center points.
            if (closedOff || (closeOffIndeces.size() > 0 && i == closeOffIndeces.front()))
            {
                const uint3 triangle_1 = {idx, (idx + 1 + pointsPerLevel), idx + 1};
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

            const uint3 triangle_1 = {idx, idx + 1 + pointsPerLevel, idx + 1};
            const uint3 triangle_2 = {idx, idx + pointsPerLevel, idx + 1 + pointsPerLevel};

            result.connectivity.push_back(triangle_1);
            result.connectivity.push_back(triangle_2);
        }
        if (closedOff)
            closeOffIndeces.erase(closeOffIndeces.begin());
    }
    result.fill_empty_field();
    return result;
}