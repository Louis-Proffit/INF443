#pragma once

#include <iostream>
#include "vcl/vcl.hpp"
#include "helpers/scene_helper.hpp"
#include "L_systems/tree_LSys.hpp"
#include "water/water.hpp"
#include "water/waterfbuffer.hpp"

class city : public environement
{
public:
    typedef environement super;
    float texture_unit_length = 0.1f;
    float h_max = 0.5f;
    float h_min = 0.1f;
    float eps = 0.001;
    float city_proportion = 0.7f;
    float border_road_proportion = 0.75f;
    float border_sidewalk_proportion = 0.77f;
    float profile_transition_down = 0.75;
    float profile_transition_up = 0.85;
    float skybox_radius = 20.0f;

    std::vector<std::vector<vcl::vec3>> patepos;

    vcl::mesh ground;
    vcl::mesh batiments;
    vcl::mesh roads;
    vcl::mesh parcs;

    vcl::mesh_drawable d_ground;
    vcl::mesh_drawable d_bat;
    vcl::mesh_drawable d_roads;
    vcl::mesh_drawable d_parcs;
    vcl::mesh_drawable sand_visual;
    vcl::mesh_drawable sidewalk_visual;
    vcl::mesh_drawable car_visual;
    TreeGenerator tree_classic;
    TreeGenerator tree_cool;
    TreeGenerator tree_real;
    std::vector<tree_located> trees;

    city(user_parameters *_user, std::function<void(scene_type)> swap_function);
    void display_visual();
    void update_visual();
    void display_interface();

    void init_pate();
    void init_ground();
    void init_tour();
    void init_sidewalk();
    void init_sand();
    void init_car();
    void compute_pate(int nb);
    void create_city(std::vector<vcl::vec3> coords);
    void merge_pat(std::vector<std::vector<vcl::vec3>> &base, std::vector<std::vector<vcl::vec3>> &to_add);
    void add_tree_positions(std::vector<tree_located> &to_add);
    vcl::vec3 random_divider();
    vcl::mesh compute_batiment(std::vector<vcl::vec3> coords);
    vcl::mesh compute_garden(std::vector<vcl::vec3> coords);
    vcl::mesh compute_windows_on_cube(vcl::vec3 const &p000, vcl::vec3 const &p100, vcl::vec3 const &p110, vcl::vec3 const &p010, vcl::vec3 const &p001, vcl::vec3 const &p101, vcl::vec3 const &p111, vcl::vec3 const &p011);
    vcl::mesh compute_windows_on_quadrangle(vcl::vec3 const &p00, vcl::vec3 const &p10, vcl::vec3 const &p11, vcl::vec3 const &p01);
    vcl::mesh compute_road_partial(vcl::vec3 const &p00, vcl::vec3 const &p10, vcl::vec3 const &p11, vcl::vec3 const &p01, bool const &is_special);
    std::vector<std::vector<vcl::vec3>> subdivise(std::vector<vcl::vec3> pate);

    float get_altitude(vcl::vec2 const &new_position_in_plane);
    float profile(vcl::vec2 const &new_position_in_plane);
    vcl::affine_rts get_car_transform(float t);

    // Water Render
    void set_water();
    Water wat;
    WaterFrameBuffers fbos;
    vec4 clipPlane = vec4(0, 0, 0, 0);

    void display_scene(vec4 clipPlane);
    void display_reflec_refrac(vec4 clipPlane);
};