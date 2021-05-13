#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "tree_LSys.hpp"

using namespace vcl;
using namespace std;

class town
{
public:
    // Vector de vector (avec 4 positions dans le deuxieme, pour avoir les positions d'un batiment dans le sens trigo)
    vector<vector<vec3>> patepos;
    mesh ground;
    mesh batiments;
    mesh roads;

    mesh_drawable d_ground;
    mesh_drawable d_bat;
    mesh_drawable d_roads;

    // à verifier sur l'image
    vec3 pixGround = vec3(22, 22, 22) / 255.0f;

    //pour la generation finale
    bool display_water = false;

    void init_pate();

    vector<vector<vec3>> subdivise(vector<vec3> pate);

    void compute_pate(int nb);

    mesh create_pate(vector<vec3> coords);

    mesh compute_batiment(vector<vec3> coords);

    mesh compute_garden(vector<vec3> coords);

    void merge_pat(vector<vector<vec3>> &base, vector<vector<vec3>> to_add);

    void init_town();

    void init_pate_water();

    mesh compute_windows_on_cube(vec3 const &p000, vec3 const &p100, vec3 const &p110, vec3 const &p010, vec3 const &p001, vec3 const &p101, vec3 const &p111, vec3 const &p011);

    mesh compute_windows_on_quadrangle(vec3 const &p00, vec3 const &p10, vec3 const &p11, vec3 const &p01);

    mesh compute_road_partial(vec3 const &p00, vec3 const &p10, vec3 const &p11, vec3 const &p01, bool const &isspecial);

    vec3 radom_divider();

    // Parametres: equation cartesienne de droite ax+by+c =0
    vector<vec3> getIntersection(float a, float b, float c);

    template <typename SCENE>
    void draw_town(SCENE const &scene)
    {
        vec3 light = scene.camera.position();
        glUseProgram(d_bat.shader);
        opengl_uniform(d_bat.shader, "projection", scene.projection);
        opengl_uniform(d_bat.shader, "view", scene.camera.matrix_view());
        opengl_uniform(d_bat.shader, "light", light);
        draw(d_bat, scene);
        //draw_wireframe(d_bat, scene);

        glUseProgram(d_roads.shader);
        opengl_uniform(d_roads.shader, "projection", scene.projection);
        opengl_uniform(d_roads.shader, "view", scene.camera.matrix_view());
        opengl_uniform(d_roads.shader, "light", light);
        draw(d_roads, scene);
        //draw_wireframe(d_roads, scene);
    }
};