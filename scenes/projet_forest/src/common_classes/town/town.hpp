#pragma once

#include "vcl/vcl.hpp"
#include <iostream>

using namespace vcl;
using namespace std;

class town
{
public:
    // Vector de vector (avec 4 positions dans le deuxieme, pour avoir les positions d'un batiment dans le sens trigo)
    vector<vector<vec3>> patepos;
    mesh ground;
    mesh batiments;
    mesh_drawable d_ground;
    mesh_drawable d_bat;

    // à verifier sur l'image
    vec3 pixGround = vec3(22, 22, 22) / 255.0f;

    //pour la generation finale
    bool display_water = false;

    void init_pate();

    vector<vector<vec3>> subdivise(vector<vec3> pate);

    void compute_pate(int nb);

    void merge_pat(vector<vector<vec3>> &base, vector<vector<vec3>> to_add);

    void init_town(string tow = "town1_low");

    vec3 radom_divider();

    template <typename SCENE>
    void draw_town(SCENE const &scene)
    {
        draw(d_bat, scene);
    }
};