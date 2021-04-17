#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
using namespace vcl;

class skybox
{
public:
    mesh face_haut;
    mesh face_gauche;
    mesh face_droite;
    mesh face_devant;
    mesh face_derriere;
    mesh_drawable dface_haut;
    mesh_drawable dface_gauche;
    mesh_drawable dface_droite;
    mesh_drawable dface_devant;
    mesh_drawable dface_derriere;

    vec3 center = vec3(0, 0, 0);
    int radius = 1;
    bool ground = false;

    // fonction à appeler dans init data
    void init_skybox(vec3 const &_center = vec3(0, 0, 0), int const &_radius = 50, std::string const &text = "sundown");

    //fonction à appeler dans draw;

    void draw_skybox();
};
