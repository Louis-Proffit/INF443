#include "town.hpp"

#include <random>
#include <iostream>

using namespace vcl;
using namespace std;

void town::init_town(string tow)
{
    init_pate();
    compute_pate(4);
    std::cout << patepos.size() << std::endl;
    for (size_t i = 0; i < patepos.size(); i++)
    {
        batiments.push_back(mesh_primitive_quadrangle(patepos[i][0], patepos[i][3], patepos[i][2], patepos[i][1]));
        /*std::cout << patepos[i][0] << std::endl;
        std::cout << patepos[i][1] << std::endl;
        std::cout << patepos[i][2] << std::endl;
        std::cout << patepos[i][3] << std::endl;
        std::cout << " " << std::endl;*/
    }

    d_bat = mesh_drawable(batiments);
}

void town::init_pate()
{
    vector<vec3> initial;
    initial.push_back(vec3(-10.0f, -10.0f, 0));
    initial.push_back(vec3(-8.0f, 8.0f, 0));
    initial.push_back(vec3(10.0f, 10.0f, 0));
    initial.push_back(vec3(5.0f, -5.0f, 0));
    patepos.push_back(initial);
}

vector<vector<vec3>> town::subdivise(vector<vec3> pate)
{
    vector<vector<vec3>> res;
    vector<vec3> current_cube;
    /*
    Avec des data dans cet ordre 
    som0 // som1
    som3 // som2 
    */
    vec3 som0 = pate[0];
    vec3 som1 = pate[1];
    //vec3 som2 = pate[2];
    vec3 som3 = pate[3];

    vec3 translatory = 0.52 * (som1 - som0);
    vec3 translatorx = 0.52 * (som3 - som0);

    /*
    Pour generer la configuration
    som0 // nvxs1
    nvxs3 // nvxs2
    */
    vec3 nvxs0 = som0;
    vec3 nvxs1 = som0 + 0.48 * (som1 - som0);
    vec3 nvxs3 = som0 + 0.48 * (som3 - som0);
    vec3 nvxs2 = nvxs1 + nvxs3 - som0;

    current_cube.push_back(nvxs0);
    current_cube.push_back(nvxs1);
    current_cube.push_back(nvxs2);
    current_cube.push_back(nvxs3);
    res.push_back(current_cube);

    current_cube.clear();

    nvxs0 += translatory;
    nvxs1 += translatory;
    nvxs2 += translatory;
    nvxs3 += translatory;

    current_cube.push_back(nvxs0);
    current_cube.push_back(nvxs1);
    current_cube.push_back(nvxs2);
    current_cube.push_back(nvxs3);
    res.push_back(current_cube);

    current_cube.clear();

    nvxs0 += translatorx;
    nvxs1 += translatorx;
    nvxs2 += translatorx;
    nvxs3 += translatorx;

    current_cube.push_back(nvxs0);
    current_cube.push_back(nvxs1);
    current_cube.push_back(nvxs2);
    current_cube.push_back(nvxs3);
    res.push_back(current_cube);

    current_cube.clear();

    nvxs0 -= translatory;
    nvxs1 -= translatory;
    nvxs2 -= translatory;
    nvxs3 -= translatory;

    current_cube.push_back(nvxs0);
    current_cube.push_back(nvxs1);
    current_cube.push_back(nvxs2);
    current_cube.push_back(nvxs3);
    res.push_back(current_cube);

    current_cube.clear();

    return res;
}

void town::merge_pat(vector<vector<vec3>> &base, vector<vector<vec3>> to_add)
{
    for (size_t i = 0; i < to_add.size(); i++)
    {
        base.push_back(to_add[i]);
    }
}

void town::compute_pate(int nb)
{
    vector<vector<vec3>> base = patepos;
    for (int j = 0; j < nb; j++)
    {
        vector<vector<vec3>> current;
        for (size_t i = 0; i < base.size(); i++)
        {
            merge_pat(current, subdivise(base[i]));
        }
        base.clear();
        merge_pat(base, current);
        current.clear();
    }
    patepos = base;
}