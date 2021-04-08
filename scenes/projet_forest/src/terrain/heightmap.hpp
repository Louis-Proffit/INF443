#pragma once

#include "vcl/vcl.hpp"
#include <iostream>

class Heightmap
{
public:
    struct HillAlgorithmParameters
    {
        //Constructeur de la struct

        HillAlgorithmParameters(int rows, int columns, int numHills, int hillRadiusMin, int hillRadiusMax, float hillMinHeight, float hillMaxHeight)
        {
            this->rows = rows;
            this->columns = columns;
            this->numHills = numHills;
            this->hillRadiusMin = hillRadiusMin;
            this->hillRadiusMax = hillRadiusMax;
            this->hillMinHeight = hillMinHeight;
            this->hillMaxHeight = hillMaxHeight;
        }

        int rows;
        int columns;
        int numHills;
        int hillRadiusMin;
        int hillRadiusMax;
        float hillMinHeight;
        float hillMaxHeight;
    };

    // Constructeur de la class

    Heightmap(const HillAlgorithmParameters &params, bool withPositions = true, bool withTextureCoordinates = true, bool withNormals = true);

    void createFromHeightData(const std::vector<std::vector<float>> &heightData);

    int getRows() const;
    int getColumns() const;
    float getHeight(const int row, const int column) const;

    static std::vector<std::vector<float>> generateRandomHeightData(const HillAlgorithmParameters &params);

private:
    void setUpVertices();
    void setUpTextureCoordinates();
    void setUpNormals();
    void setUpIndexBuffer();

    std::vector<std::vector<float>> _heightData;
    std::vector<int> _vertices;
    std::vector<int> _textureCoordinates;
    std::vector<vcl::vec3> _normals;
    int _rows = 0;
    int _columns = 0;
    bool _isInitialized = false;
};