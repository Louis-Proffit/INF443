#pragma once

#include "vcl/vcl.hpp"
#include <iostream>

using namespace vcl;

class Water
{
public:
    float waterHeight = 0;
    mesh grid;

    void init_water();
};