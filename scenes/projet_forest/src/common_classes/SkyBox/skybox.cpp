#include "skybox.hpp"

#include <random>
#include <iostream>
#include <string>

using namespace vcl;

void skybox::init_skybox(vec3 const &_center, int const &_radius, std::string const &text)
{

    this->center = _center;
    this->radius = _radius;
    image_raw haut;
    image_raw gauche;
    image_raw droite;
    image_raw devant;
    image_raw derriere;
    image_raw dessous;
    if (text == "desert")
    {
        /*haut = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/desert/top.png");
        gauche = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/desert/left.png");
        droite = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/desert/right.png");
        devant = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/desert/front.png");
        derriere = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/desert/back.png");*/
        haut = image_load_png("assets/skyboxes/desert/top.png");
        gauche = image_load_png("assets/skyboxes/desert/left.png");
        droite = image_load_png("assets/skyboxes/desert/right.png");
        devant = image_load_png("assets/skyboxes/desert/front.png");
        derriere = image_load_png("assets/skyboxes/desert/back.png");
    }
    else if (text == "sundown")
    {
        /*haut = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/sundown/top.png");
        gauche = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/sundown/left.png");
        droite = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/sundown/right.png");
        devant = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/sundown/front.png");
        derriere = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/sundown/back.png");
        dessous = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/sundown/bottom.png");*/
        haut = image_load_png("assets/skyboxes/sundown/top.png");
        gauche = image_load_png("assets/skyboxes/sundown/left.png");
        droite = image_load_png("assets/skyboxes/sundown/right.png");
        devant = image_load_png("assets/skyboxes/sundown/front.png");
        derriere = image_load_png("assets/skyboxes/sundown/back.png");
        dessous = image_load_png("assets/skyboxes/sundown/bottom.png");
        this->ground = true;
    }
    else if (text == "space")
    {   /*haut = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/space/top.png");
        gauche = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/space/left.png");
        droite = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/space/right.png");
        devant = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/space/front.png");
        derriere = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/space/back.png");
        dessous = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/space/bottom.png");*/
        haut = image_load_png("assets/skyboxes/space/top.png");
        gauche = image_load_png("assets/skyboxes/space/left.png");
        droite = image_load_png("assets/skyboxes/space/right.png");
        devant = image_load_png("assets/skyboxes/space/front.png");
        derriere = image_load_png("assets/skyboxes/space/back.png");
        dessous = image_load_png("assets/skyboxes/space/bottom.png");
        this->ground = true;
    }

    else if (text == "neige")
    {
        /*haut = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/neige/top.png");
        gauche = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/neige/left.png");
        droite = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/neige/right.png");
        devant = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/neige/front.png");
        derriere = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/neige/back.png");
        dessous = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/neige/bottom.png");*/
        haut = image_load_png("assets/skyboxes/neige/top.png");
        gauche = image_load_png("assets/skyboxes/neige/left.png");
        droite = image_load_png("assets/skyboxes/neige/right.png");
        devant = image_load_png("assets/skyboxes/neige/front.png");
        derriere = image_load_png("assets/skyboxes/neige/back.png");
        dessous = image_load_png("assets/skyboxes/neige/bottom.png");
        this->ground = true;
    }
    else if (text == "fleuve")
    {
        /*haut = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/fleuve/top.png");
        gauche = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/fleuve/left.png");
        droite = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/fleuve/right.png");
        devant = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/fleuve/front.png");
        derriere = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/fleuve/back.png");
        dessous = image_load_png("/Users/paultheron/Desktop/Projet2/INF443/scenes/projet_forest/assets/skyboxes/fleuve/bottom.png");*/
        haut = image_load_png("assets/skyboxes/fleuve/top.png");
        gauche = image_load_png("assets/skyboxes/fleuve/left.png");
        droite = image_load_png("assets/skyboxes/fleuve/right.png");
        devant = image_load_png("assets/skyboxes/fleuve/front.png");
        derriere = image_load_png("assets/skyboxes/fleuve/back.png");
        dessous = image_load_png("assets/skyboxes/fleuve/bottom.png");
        this->ground = true;
    }

    else
    {
        std::cout << "la texture n'existe pas" << std::endl;
    }

    // front face;
    face_devant = mesh_primitive_grid(vec3(-radius, -radius, radius), vec3(-radius, -radius, -radius), vec3(-radius, radius, -radius), vec3(-radius, radius, radius), 2, 2);
    dface_devant = mesh_drawable(face_devant);
    dface_devant.texture = opengl_texture_to_gpu(devant, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    /*dface_devant.shading.phong = {1, 0, 0, 0};*/

    //back face
    face_derriere = mesh_primitive_grid(vec3(radius, radius, radius), vec3(radius, radius, -radius), vec3(radius, -radius, -radius), vec3(radius, -radius, radius), 2, 2);
    dface_derriere = mesh_drawable(face_derriere);
    dface_derriere.texture = opengl_texture_to_gpu(derriere, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
   /* dface_derriere.shading.phong = {1, 0, 0, 0};*/

    //face droite
    face_droite = mesh_primitive_grid(vec3(-radius, radius, radius), vec3(-radius, radius, -radius), vec3(radius, radius, -radius), vec3(radius, radius, radius), 2, 2);
    dface_droite = mesh_drawable(face_droite);
    dface_droite.texture = opengl_texture_to_gpu(droite, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
   /* dface_droite.shading.phong = {1, 0, 0, 0};*/

    //face gauche
    face_gauche = mesh_primitive_grid(vec3(radius, -radius, radius), vec3(radius, -radius, -radius), vec3(-radius, -radius, -radius), vec3(-radius, -radius, radius), 2, 2);
    dface_gauche = mesh_drawable(face_gauche);
    dface_gauche.texture = opengl_texture_to_gpu(gauche, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    /*dface_gauche.shading.phong = {1, 0, 0, 0};*/

    //dessus
    face_haut = mesh_primitive_grid(vec3(radius, -radius, radius), vec3(-radius, -radius, radius), vec3(-radius, radius, radius), vec3(radius, radius, radius), 2, 2);
    dface_haut = mesh_drawable(face_haut);
    dface_haut.texture = opengl_texture_to_gpu(haut, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    dface_haut.shading.phong = {1, 0, 0, 0};

    if (this->ground)
    { // sol
        face_dessous = mesh_primitive_grid(vec3(radius, -radius, -radius), vec3(-radius, -radius, -radius), vec3(-radius, radius, -radius), vec3(radius, radius, -radius), 2, 2);
        dface_dessous = mesh_drawable(face_dessous);
        dface_dessous.texture = opengl_texture_to_gpu(dessous, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        /*dface_dessous.shading.phong = {1, 0, 0, 0};*/
    }
}
