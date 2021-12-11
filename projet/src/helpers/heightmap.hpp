#pragma once

#include "vcl/vcl.hpp"
#include <iostream>

struct random_heightmap_parameters
{
    //Constructeur de la struct
    int rows;
    int columns;
    int numHills;
    int hill_radius_min;
    int hill_radius_max;
    float hill_min_height;
    float hill_max_height;

    random_heightmap_parameters(int rows = 0, int columns = 0, int numHills  = 0, int hillRadiusMin = 0, int hillRadiusMax = 0, float hillMinHeight = 0, float hillMaxHeight = 2.5)
    {
        this->rows = rows;
        this->columns = columns;
        this->numHills = numHills;
        this->hill_radius_min = hillRadiusMin;
        this->hill_radius_max = hillRadiusMax;
        this->hill_min_height = hillMinHeight;
        this->hill_max_height = hillMaxHeight;
    }
};

struct heightmap_parameters {
    int rows;               // usefull on return
    int columns;            // usefull on return
    float min_x = -1.0f;    // needed as input
    float min_y = -1.0f;    // needed as input
    float max_x = 1.0f;     // needed as input
    float max_y = 1.0f;     // needed as input
};

vcl::mesh createFromHeightData(const std::vector<std::vector<float>> &heightDatae, heightmap_parameters& params);

std::vector<std::vector<float>> generateRandomHeightData(const random_heightmap_parameters &params);

std::vector<std::vector<float>> generateFileHeightData(const std::string &filename, float horizontal_scale = 1.0);
