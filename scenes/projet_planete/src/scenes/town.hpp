#pragma once

#include "vcl/vcl.hpp"
#include <iostream>

#include "scene_helper.hpp"
#include "constants.hpp"

class city : public scene_visual
{
public:
    // Vector de vector (avec 4 positions dans le deuxieme, pour avoir les positions d'un batiment dans le sens trigo)
    std::vector<std::vector<vcl::vec3>> patepos;
    vcl::mesh ground;
    vcl::mesh batiments;
    vcl::mesh_drawable d_ground;
    vcl::mesh_drawable d_bat;

    // à verifier sur l'image
    vcl::vec3 pixGround = vcl::vec3(22, 22, 22) / 255.0f;

    //pour la generation finale
    bool display_water = false;

    city(user_parameters* _user, std::function<void(scene_type)> swap_function);
    ~city();

    void display_visual();
    void update_visual();
    void display_interface();

    void init_pate();
    std::vector<std::vector<vcl::vec3>> subdivise(std::vector<vcl::vec3> pate);
    void compute_pate(int nb);
    vcl::mesh create_pate(std::vector<vcl::vec3> coords);
    vcl::mesh compute_batiment(std::vector<vcl::vec3> coords);
    vcl::mesh compute_garden(std::vector<vcl::vec3> coords);
    void merge_pat(std::vector<std::vector<vcl::vec3>> &base, std::vector<std::vector<vcl::vec3>> to_add);
    void init_city(std::string tow = "city1_low");
    vcl::vec3 random_divider();
};