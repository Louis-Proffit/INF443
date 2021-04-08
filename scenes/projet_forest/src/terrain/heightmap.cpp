#include "heightmap.hpp"

#include <random>
#include <iostream>

using namespace vcl;

std::vector<std::vector<float>> Heightmap::generateRandomHeightData(const HillAlgorithmParameters &params)
{

    std::vector<std::vector<float>> heightData(params.rows, std::vector<float>(params.columns, 0.0f)); // Constructor de la class vector pour avoir un vector de vectors (matrice) tous initialisés à 0

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> hillRadiusDistribution(params.hillRadiusMin, params.hillRadiusMax);
    std::uniform_real_distribution<> hillHeightDistribution(params.hillMinHeight, params.hillMaxHeight);
    std::uniform_int_distribution<> hillCenterRowIntDistribution(0, params.rows - 1);
    std::uniform_int_distribution<> hillCenterColIntDistribution(0, params.columns - 1);

    for (int i = 0; i < params.numHills; i++)
    {
        int hillCenterRow = hillCenterRowIntDistribution(generator);
        int hillCenterCol = hillCenterColIntDistribution(generator);
        int hillRadius = hillRadiusDistribution(generator);
        float hillHeight = hillHeightDistribution(generator);

        for (int r = hillCenterRow - hillRadius; r < hillCenterRow + hillRadius; r++)
        {
            for (int c = hillCenterCol - hillRadius; c < hillCenterCol + hillRadius; c++)
            {
                if (r < 0 || r >= params.rows || c < 0 || c >= params.columns)
                {
                    continue;
                }
                int r2 = hillRadius * hillRadius; // r*r term
                int x2x1 = hillCenterCol - c;     // (x2-x1) term
                int y2y1 = hillCenterRow - r;     // (y2-y1) term
                float height = float(r2 - x2x1 * x2x1 - y2y1 * y2y1);
                if (height < 0.0f)
                {
                    continue;
                }
                float factor = height / r2;
                heightData[r][c] = std::max(hillHeight * factor, heightData[r][c]);
                if (heightData[r][c] > 1.0f)
                {
                    heightData[r][c] = 1.0f;
                }
            }
        }
    }
    return heightData;
}

void Heightmap::createFromHeightData(const std::vector<std::vector<float>> &heightData)
{
    if (_isInitialized)
    {
        deleteMesh();
    }

    _heightData = heightData;
    _rows = _heightData.size();
    _columns = _heightData[0].size();
    _numVertices = _rows * _columns;

    // First, prepare VAO and VBO for vertex data
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    _vbo.createVBO(_numVertices * getVertexByteSize()); // Preallocate memory
    _vbo.bindVBO();

    if (hasPositions())
    {
        setUpVertices();
    }

    if (hasTextureCoordinates())
    {
        setUpTextureCoordinates();
    }

    if (hasNormals())
    {
        if (!hasPositions())
        {
            setUpVertices();
        }

        setUpNormals();
    }

    setUpIndexBuffer();

    // Clear the data, we won't need it anymore
    _vertices.clear();
    _textureCoordinates.clear();
    _normals.clear();

    // If get here, we have succeeded with generating heightmap
    _isInitialized = true;
}