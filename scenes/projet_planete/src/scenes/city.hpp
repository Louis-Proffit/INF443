#pragma once

#include <iostream>
#include "vcl/vcl.hpp"
#include "scene_helper.hpp"
#include "skybox.hpp"

class city : public scene_visual
{
public:

    float x_min = -2.0f;
    float y_min = -2.0f;
    float x_max = 2.0f;
    float y_max = 2.0f;
    float texture_unit_length = 0.1f;
    float h_max = 0.1f;
    float h_min = 0.05f;
    float eps = 0.001;
    float skybox_radius = 5.0f;

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

    city(user_parameters* _user, std::function<void(scene_type)> swap_function);
    ~city();
    void display_visual();
    void update_visual();
    void display_interface();

    void init_city();
    void init_pate_water();
    void init_pate();
    void init_ground();
    void compute_pate(int nb);
    void create_city(std::vector<vcl::vec3> coords);
    void merge_pat(std::vector<std::vector<vcl::vec3>>& base, std::vector<std::vector<vcl::vec3>> to_add);
    vcl::vec3 random_divider();
    vcl::mesh compute_batiment(std::vector<vcl::vec3> coords);
    vcl::mesh compute_garden(std::vector<vcl::vec3> coords);
    vcl::mesh compute_windows_on_cube(vcl::vec3 const& p000, vcl::vec3 const& p100, vcl::vec3 const& p110, vcl::vec3 const& p010, vcl::vec3 const& p001, vcl::vec3 const& p101, vcl::vec3 const& p111, vcl::vec3 const& p011);
    vcl::mesh compute_windows_on_quadrangle(vcl::vec3 const& p00, vcl::vec3 const& p10, vcl::vec3 const& p11, vcl::vec3 const& p01);
    vcl::mesh compute_road_partial(vcl::vec3 const& p00, vcl::vec3 const& p10, vcl::vec3 const& p11, vcl::vec3 const& p01, bool const& isspecial);
    std::vector<std::vector<vcl::vec3>> subdivise(std::vector<vcl::vec3> pate);
    std::vector<vcl::vec3> getIntersection(float a, float b, float c);// Parametres: equation cartesienne de droite ax+by+c =0
    float city::get_altitude(vcl::vec2 const& new_position_in_plane);
};