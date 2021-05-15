#pragma once

#include <iostream>
#include "vcl/vcl.hpp"
#include "scene_helper.hpp"
#include "skybox.hpp"

class city : public scene_visual
{
public:
    //std::vector destd::vector (avec 4 positions dans le deuxieme, pour avoir les positions d'un batiment dans le sens trigo)
    std::vector<std::vector<vcl::vec3>> patepos;

    vcl::mesh ground;
    vcl::mesh batiments;
    vcl::mesh roads;
    vcl::mesh parcs;

    skybox box;

    vcl::mesh_drawable d_ground;
    vcl::mesh_drawable d_bat;
    vcl::mesh_drawable d_roads;
    vcl::mesh_drawable d_parcs;

    // Camera
    vcl::camera_around_center camera_c;
    vcl::camera_minecraft camera_m;
    bool m_activated;
    
    vcl::vec3 pixGround = vcl::vec3(22, 22, 22) / 255.0f;// à verifier sur l'image
    bool display_water = false;//pour la generation finale

    city(user_parameters* _user, std::function<void(scene_type)> swap_function);
    ~city();
    void display_visual();
    void update_visual();
    void display_interface();


    void init_pate();
    std::vector<std::vector<vcl::vec3>> subdivise(std::vector<vcl::vec3> pate);
    void compute_pate(int nb);
    void create_city(std::vector<vcl::vec3> coords);
    vcl::mesh compute_batiment(std::vector<vcl::vec3> coords);
    vcl::mesh compute_garden(std::vector<vcl::vec3> coords);
    void merge_pat(std::vector<std::vector<vcl::vec3>>& base,std::vector<std::vector<vcl::vec3>> to_add);
    void init_city();
    void init_pate_water();
    vcl::mesh compute_windows_on_cube(vcl::vec3 const& p000, vcl::vec3 const& p100, vcl::vec3 const& p110, vcl::vec3 const& p010, vcl::vec3 const& p001, vcl::vec3 const& p101, vcl::vec3 const& p111, vcl::vec3 const& p011);
    vcl::mesh compute_windows_on_quadrangle(vcl::vec3 const& p00, vcl::vec3 const& p10, vcl::vec3 const& p11, vcl::vec3 const& p01);
    vcl::mesh compute_road_partial(vcl::vec3 const& p00, vcl::vec3 const& p10, vcl::vec3 const& p11, vcl::vec3 const& p01, bool const& isspecial);
    vcl::vec3 random_divider();
    std::vector<vcl::vec3> getIntersection(float a, float b, float c);// Parametres: equation cartesienne de droite ax+by+c =0
    float city::get_altitude(vcl::vec2 const& new_position_in_plane);
};