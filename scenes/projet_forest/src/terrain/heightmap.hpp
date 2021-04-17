#pragma once

#include "vcl/vcl.hpp"
#include <iostream>

struct HillAlgorithmParameters
{
    //Constructeur de la struct
    int rows;
    int columns;
    int numHills;
    int hillRadiusMin;
    int hillRadiusMax;
    float hillMinHeight;
    float hillMaxHeight;

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

    HillAlgorithmParameters()
    {
        this->rows = 0;
        this->columns = 0;
        this->numHills = 0;
        this->hillRadiusMin = 0;
        this->hillRadiusMax = 0;
        this->hillMinHeight = 0;
        this->hillMaxHeight = 1.5;
    }
};

vcl::mesh createFromHeightData(const std::vector<std::vector<float>> &heightData, const HillAlgorithmParameters &params);

std::vector<std::vector<float>> generateRandomHeightData(const HillAlgorithmParameters &params);

std::vector<std::vector<float>> generateFileHeightData(const std::string &filename, HillAlgorithmParameters &params);
