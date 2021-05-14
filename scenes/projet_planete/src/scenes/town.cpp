#include "town.hpp"

#include <random>
#include <iostream>
#include <cmath>

using namespace vcl;
using namespace std;

city::city(user_parameters* _user, std::function<void(scene_type)> _swap_function) : scene_visual(_user, _swap_function)
{
    user_reference = _user;
    swap_function = _swap_function;
    init_city();
}

city::~city()
{
}

void city::display_visual()
{
    draw(d_bat, this);
    draw(d_ground, this);
    if (user_reference->draw_wireframe)
    {
        draw_wireframe(d_bat, this);
        draw_wireframe(d_ground, this);
    }
}

void city::update_visual()
{
    // Rien a update
    vec2 const& p1 = user_reference->mouse_curr;
}

void city::display_interface()
{
    ImGui::Checkbox("Frame", &user_reference->display_frame);
    ImGui::Checkbox("Wireframe", &user_reference->draw_wireframe);
    if (ImGui::Button("Retour à la planète")) swap_function(scene_type::PLANET);
}

void city::init_city(string tow)
{
    init_pate();
    compute_pate(4);
    std::cout << patepos.size() << std::endl;
    for (size_t i = 0; i < patepos.size(); i++)
    {
        //batiments.push_back(mesh_primitive_quadrangle(patepos[i][0], patepos[i][3], patepos[i][2], patepos[i][1]));
        batiments.push_back(create_pate(patepos[i]));
        /*std::cout << patepos[i][0] << std::endl;
        std::cout << patepos[i][1] << std::endl;
        std::cout << patepos[i][2] << std::endl;
        std::cout << patepos[i][3] << std::endl;
        std::cout << " " << std::endl;*/
    }

    d_bat = mesh_drawable(batiments, open_shader("normal"));
}

void city::init_pate()
{
    vector<vec3> initial;
    initial.push_back(vec3(-10.0f, -10.0f, 0));
    initial.push_back(vec3(-10.0f, 10.0f, 0));
    initial.push_back(vec3(10.0f, 10.0f, 0));
    initial.push_back(vec3(10.0f, -10.0f, 0));
    initial.push_back(vec3(0, 0, 0));
    patepos.push_back(initial);
}

vec3 city::random_divider()
{
    float al = (rand() / (float)RAND_MAX);
    if (al < 0.02f)
    {
        return vec3(1, 0, 0);
    }
    else if (al < 0.06f)
    {
        return vec3(2, 0, 0);
    }
    else
        return vec3(0, 0, 0);
}
/*
Note sur les differents types de subdivision, elles sont codées par un vec3 dans pate qui contient donc 5 elements 

On note pour les types:
Vec3(0,0,0) -> division normale -> on renvoit la division en 4
Vec3(1,0,0) -> création d'un parc -> on arrete à jamais les divisons
Vec3(2,0,0) -> division en cercle pour cette etape

Avoir 1 en coordonnée y signifie qu'on peut pas avoir de cercle à l'interieur
*/
vector<vector<vec3>> city::subdivise(vector<vec3> pate)
{
    vector<vector<vec3>> res;
    if (pate[4].x == 0)
    {
        vector<vec3> current_cube;
        /*
    Avec des data dans cet ordre 
    som0 // som1
    som3 // som2 
    */
        vec3 som0 = pate[0];
        vec3 som1 = pate[1];
        vec3 som2 = pate[2];
        vec3 som3 = pate[3];

        //vec3 translatory = 0.52 * (som1 - som0);
        //vec3 translatorx = 0.52 * (som3 - som0);

        /*
    Pour generer la configuration
    som0 // nvxs1
    nvxs3 // nvxs2
    */
        vec3 nvxs0 = som0;
        vec3 nvxs1 = som0 + 0.48 * (som1 - som0);
        vec3 nvxs2 = nvxs1 + 0.48 * (som3 - som0);
        vec3 nvxs3 = som0 + 0.48 * (som3 - som0);

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = som1 + 0.48 * (som0 - som1);
        nvxs1 = som1;
        nvxs2 = som1 + 0.48 * (som2 - som1);
        nvxs3 = nvxs2 + 0.48 * (som3 - som2);

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs1 = som2 + 0.48 * (som1 - som2);
        nvxs0 = nvxs1 + 0.48 * (som3 - som2);
        nvxs2 = som2;
        nvxs3 = som2 + 0.48 * (som3 - som2);

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = som3 + 0.48 * (som0 - som3);
        nvxs1 = nvxs0 + 0.48 * (som1 - som0);
        nvxs2 = som3 + 0.48 * (som2 - som3);
        nvxs3 = som3;

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
        res.push_back(current_cube);

        current_cube.clear();
    }

    if (pate[4].x == 1)
    {
        res.push_back(pate);
    }
    else if (pate[4].x == 2)
    {
        vector<vec3> current_cube;
        float decal = PI / 64;
        float decalh = 0.03f;
        vec3 som0 = pate[0];
        vec3 som1 = pate[1];
        vec3 som2 = pate[2];
        vec3 som3 = pate[3];

        vec3 center = (som1 + som3 + som2 + som0) / 4;
        //float rad = norm(((som1 + som0) / 2 - center));
        vec3 xloc = -((som3 + som0) / 2 - center);
        vec3 yloc = (som1 + som0) / 2 - center;

        vec3 nvxs0 = center + cos(3 * PI / 4 - decal) * xloc + sin(3 * PI / 4 - decal) * yloc;
        vec3 nvxs1 = center + yloc;
        vec3 nvxs2 = center + cos(PI / 4 + decal) * xloc + sin(PI / 4 + decal) * yloc;
        vec3 nvxs3 = center + decalh * yloc;
        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(vec3(0, 1, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = center + cos(PI / 4 - decal) * xloc + sin(PI / 4 - decal) * yloc;
        nvxs1 = center + xloc;
        nvxs2 = center + cos(-PI / 4 + decal) * xloc + sin(-PI / 4 + decal) * yloc;
        nvxs3 = center + decalh * xloc;
        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(vec3(0, 1, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = center + cos(-PI / 4 - decal) * xloc + sin(-PI / 4 - decal) * yloc;
        nvxs1 = center - yloc;
        nvxs2 = center + cos(-3 * PI / 4 + decal) * xloc + sin(-3 * PI / 4 + decal) * yloc;
        nvxs3 = center - decalh * yloc;
        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(vec3(0, 1, 0));
        res.push_back(current_cube);

        current_cube.clear();

        nvxs0 = center + cos(-3 * PI / 4 - decal) * xloc + sin(-3 * PI / 4 - decal) * yloc;
        nvxs1 = center - xloc;
        nvxs2 = center + cos(3 * PI / 4 + decal) * xloc + sin(3 * PI / 4 + decal) * yloc;
        nvxs3 = center - decalh * xloc;
        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(vec3(0, 1, 0));
        res.push_back(current_cube);

        current_cube.clear();
    }

    return res;
}

void city::merge_pat(vector<vector<vec3>> &base, vector<vector<vec3>> to_add)
{
    for (size_t i = 0; i < to_add.size(); i++)
    {
        base.push_back(to_add[i]);
    }
}

void city::compute_pate(int nb)
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

mesh city::compute_batiment(vector<vec3> coords)
{
    float prob = ((rand() / (float)RAND_MAX));
    // formule pour avoir haut dans[a,b] haut = (b-a)*rand+a
    float haut = (0.7f) * ((rand() / (float)RAND_MAX)) + 0.5;
    mesh bat;
    vec3 som0 = coords[0];
    vec3 som1 = coords[1];
    vec3 som2 = coords[2];
    vec3 som3 = coords[3];
    vec3 up = vec3(0, 0, haut);
    if (prob < 0.7f)
    {
        bat.push_back(mesh_primitive_cubic_grid(som0, som3, som2, som1, som0 + up, som3 + up, som2 + up, som1 + up, 4, 4, 4));
        vec3 nvxs0 = (3 * som0 + som2) / 4;
        vec3 nvxs1 = (3 * som1 + som3) / 4;
        vec3 nvxs2 = (som0 + 3 * som2) / 4;
        vec3 nvxs3 = (som1 + 3 * som3) / 4;
        bat.push_back(mesh_primitive_cubic_grid(nvxs0 + up, nvxs3 + up, nvxs2 + up, nvxs1 + up, nvxs0 + up + up, nvxs3 + up + up, nvxs2 + up + up, nvxs1 + up + up, 4, 4, 4));
        nvxs0 = (3 * som0 + 2 * som2) / 5;
        nvxs1 = (3 * som1 + 2 * som3) / 5;
        nvxs2 = (2 * som0 + 3 * som2) / 5;
        nvxs3 = (2 * som1 + 3 * som3) / 5;
        bat.push_back(mesh_primitive_cubic_grid(nvxs0 + up + up, nvxs3 + up + up, nvxs2 + up + up, nvxs1 + up + up, nvxs0 + up + up + up, nvxs3 + up + up + up, nvxs2 + up + up + up, nvxs1 + up + up + up, 4, 4, 4));
        for (auto i = 0; i < bat.color.size(); i++)
        {
            bat.color[i] = vec3(0.8f, 0.8f, 0.8f);
        }
    }
    else if (prob < 0.9f)
    {
        bat.push_back(mesh_primitive_cubic_grid(som0, som3, som2, som1, som0 + 2 * up, som3 + 2 * up, som2 + 2 * up, som1 + 2 * up, 4, 4, 4));
    }
    else
    {
        vec3 center = (som0 + som1 + som2 + som3) / 4;
        mesh cylindre = mesh_primitive_cylinder(3 * norm((som0 + som1) / 2 - center) / 4, center, center + 2.5f * up, 20, 10);
        mesh cone = mesh_primitive_cone(3 * norm((som0 + som1) / 2 - center) / 4, 0.4 * haut, center + 2.5f * up);
        bat.push_back(cylindre);
        bat.push_back(cone);
    }
    return bat;
}

mesh city::compute_garden(vector<vec3> coords)
{
    // Pour compute les arbres, generer un vecteur de positions d'arbres et de hauteur, qui sera ensuite executé au moment du draw
    mesh sol = mesh_primitive_quadrangle(coords[0], coords[3], coords[2], coords[1]);
    sol.color.fill(vec3(0, 1.0f, 0));
    return sol;
}

mesh city::create_pate(vector<vec3> coords)
{
    if (coords[4].x == 0)
    {
        return compute_batiment(coords);
    }
    else if (coords[4].x == 1)
    {
        return compute_garden(coords);
    }
}