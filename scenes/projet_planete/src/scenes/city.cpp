#include "city.hpp"

#include <random>
#include <iostream>
#include <cmath>

using namespace vcl;
using namespace std;

city::city(user_parameters *_user, std::function<void(scene_type)> swap_function) : environement(_user, swap_function)
{
    // Configuration de la ville
    init_pate();
    init_ground();
    compute_pate(5);
    for (size_t i = 0; i < patepos.size(); i++)
        create_city(patepos[i]);

    batiments.fill_empty_field();
    roads.fill_empty_field();
    parcs.fill_empty_field();
    ground.fill_empty_field();

    GLuint tree_shader = scene_visual::get_shader(shader_type::TREE);
    tree_classic.initTree(tree_type::CLASSIC, tree_shader);
    tree_cool.initTree(tree_type::COOL, tree_shader);
    tree_real.initTree(tree_type::REAL_1, tree_shader);

    // Configure meshes
    GLuint normal_shader = scene_visual::get_shader(shader_type::NORMAL);
    d_bat = mesh_drawable(batiments, normal_shader);
    d_roads = mesh_drawable(roads, normal_shader);
    d_parcs = mesh_drawable(parcs, normal_shader);
    d_ground = mesh_drawable(ground, normal_shader);

    // Configure textures and colors
    d_ground.shading.color = vec3(0.5, 0.5, 0.5);
    d_parcs.texture = scene_visual::get_texture(texture_type::GRASS);
    skybox.init_skybox({0, 0, 0}, std::max(x_max - x_min, y_max - y_min), skybox_type::FLEUVE, normal_shader);

    x_min = -10.0;
    y_min = -10.0;
    x_max = 10.0;
    y_max = 10.0;

    // Camera
    camera_m.manipulator_set_altitude(get_altitude(camera_m.position_camera.xy()));
}

void city::display_visual()
{
    super::display_visual();

    glClearColor(0.256f, 0.256f, 0.256f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    vec3 light = camera_c.position();

    /* Shaders*/
    GLuint normal_shader = scene_visual::get_shader(shader_type::NORMAL);
    glUseProgram(normal_shader);
    opengl_uniform(normal_shader, "projection", projection);
    if (m_activated)
        opengl_uniform(normal_shader, "view", camera_m.matrix_view());
    else
        opengl_uniform(normal_shader, "view", camera_c.matrix_view());
    opengl_uniform(normal_shader, "light", light);

    GLuint tree_shader = scene_visual::get_shader(shader_type::TREE);
    glUseProgram(tree_shader);
    opengl_uniform(tree_shader, "projection", projection);
    if (m_activated)
        opengl_uniform(tree_shader, "view", camera_m.matrix_view());
    else
        opengl_uniform(tree_shader, "view", camera_c.matrix_view());
    opengl_uniform(tree_shader, "light", light);

    /* Trees */
    for (tree_located tree : trees) {
        switch (tree.type) {
        case tree_type::COOL:
            tree_cool.translate(tree.position);
            draw(tree_cool.dtrunk, this);
            if (tree_cool.hasleaves) draw(tree_cool.dleaves, this);
            if (user_reference->draw_wireframe) {
                draw_wireframe(tree_cool.dtrunk, this);
                if (tree_cool.hasleaves) draw_wireframe(tree_cool.dleaves, this);
            }
            break;
        case tree_type::CLASSIC:
            tree_classic.translate(tree.position);
            draw(tree_real.dtrunk, this);
            if (tree_real.hasleaves) draw(tree_real.dleaves, this);
            if (user_reference->draw_wireframe) {
                draw_wireframe(tree_real.dtrunk, this);
                if (tree_real.hasleaves) draw_wireframe(tree_real.dleaves, this);
            }
            break;
        case tree_type::REAL_1:
            tree_real.translate(tree.position);
            draw(tree_real.dtrunk, this);
            if (tree_real.hasleaves) draw(tree_real.dleaves, this);
            if (user_reference->draw_wireframe) {
                draw_wireframe(tree_real.dtrunk, this);
                if (tree_real.hasleaves) draw_wireframe(tree_real.dleaves, this);
            }
            break;
        }
    }

    draw(d_bat, this);
    if (user_reference->draw_wireframe)
        draw_wireframe(d_bat, this);
    draw(d_roads, this);
    if (user_reference->draw_wireframe)
        draw_wireframe(d_roads, this);
    draw(d_parcs, this);
    if (user_reference->draw_wireframe)
        draw_wireframe(d_parcs, this);
    draw(d_ground, this);
    if (user_reference->draw_wireframe)
        draw_wireframe(d_ground, this);
}

void city::display_interface()
{
    super::display_interface();
}

void city::update_visual()
{
    super::update_visual();
}

void city::init_pate()
{
    vector<vec3> initial;
    initial.push_back(vec3(x_min, y_min, 0));
    initial.push_back(vec3(x_min, y_max, 0));
    initial.push_back(vec3(x_max, y_max, 0));
    initial.push_back(vec3(x_max, y_min, 0));
    initial.push_back(vec3(0, 0, 0));
    patepos.push_back(initial);
}

void city::init_ground()
{
    ground.position.resize(4);
    ground.connectivity.resize(2);
    ground.uv.resize(4);
    ground.position[0] = vec3(x_min, y_min, -eps);
    ground.position[1] = vec3(x_max, y_min, -eps);
    ground.position[2] = vec3(x_max, y_max, -eps);
    ground.position[3] = vec3(x_min, y_max, -eps);
    ground.connectivity[0] = uint3(0, 1, 2);
    ground.connectivity[1] = uint3(0, 2, 3);
    ground.uv[0] = vec2(0, 0);
    ground.uv[1] = vec2((x_max - x_min) / texture_unit_length, 0);
    ground.uv[2] = vec2((x_max - x_min) / texture_unit_length, (y_max - y_min) / texture_unit_length);
    ground.uv[3] = vec2(0, (y_max - y_min) / texture_unit_length);
}

vec3 city::random_divider()
{
    float al = (rand() / (float)RAND_MAX);
    if (al < 0.02f)
    {
        return vec3(1, 0, 0);
    }
    else if (al < 0.07f)
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
Vec3(3,0,0) -> divise un rectangle en 2

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
        float decalagey = (0.3f) * rand() / (float)RAND_MAX + 0.35f;
        float decalagex = (0.3f) * rand() / (float)RAND_MAX + 0.35f;
        vec3 nvxs0 = som0;
        vec3 nvxs1 = som0 + (decalagey - 0.02f) * (som1 - som0);
        vec3 nvxs2 = nvxs1 + (decalagex - 0.02f) * (som3 - som0);
        vec3 nvxs3 = som0 + (decalagex - 0.02f) * (som3 - som0);

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
        res.push_back(current_cube);

        roads.push_back(compute_road_partial(nvxs1, nvxs2, som1 + (decalagex - 0.02f) * (som2 - som1) + (1 - 0.02f - decalagey) * (som3 - som2), som1 + (1 - 0.02f - decalagey) * (som0 - som1), false));

        current_cube.clear();

        nvxs0 = som1 + (1 - 0.02f - decalagey) * (som0 - som1);
        nvxs1 = som1;
        nvxs2 = som1 + (decalagex - 0.02f) * (som2 - som1);
        nvxs3 = nvxs2 + (1 - 0.02f - decalagey) * (som3 - som2);

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
        res.push_back(current_cube);

        current_cube.clear();
        roads.push_back(compute_road_partial(nvxs3, som2 + (1 - 0.02f - decalagex) * (som1 - som2) + (1 - decalagey - 0.02f) * (som3 - som2), som2 + (1 - 0.02f - decalagex) * (som1 - som2), nvxs2, false));

        nvxs1 = som2 + (1 - 0.02f - decalagex) * (som1 - som2);
        nvxs0 = nvxs1 + (1 - decalagey - 0.02f) * (som3 - som2);
        nvxs2 = som2;
        nvxs3 = som2 + (1 - decalagey - 0.02f) * (som3 - som2);

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
        res.push_back(current_cube);

        roads.push_back(compute_road_partial(som3 + (1 - decalagex - 0.02f) * (som0 - som3) + (decalagey - 0.02f) * (som1 - som0), som3 + (decalagey - 0.02f) * (som2 - som3), nvxs3, nvxs0, false));

        current_cube.clear();

        nvxs0 = som3 + (1 - decalagex - 0.02f) * (som0 - som3);
        nvxs1 = nvxs0 + (decalagey - 0.02f) * (som1 - som0);
        nvxs2 = som3 + (decalagey - 0.02f) * (som2 - som3);
        nvxs3 = som3;

        current_cube.push_back(nvxs0);
        current_cube.push_back(nvxs1);
        current_cube.push_back(nvxs2);
        current_cube.push_back(nvxs3);
        current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
        res.push_back(current_cube);
        roads.push_back(compute_road_partial(som0 + (decalagex - 0.02f) * (som3 - som0), nvxs0, nvxs1, som0 + (decalagey - 0.02f) * (som1 - som0) + (decalagex - 0.02f) * (som3 - som0), false));

        current_cube.clear();

        roads.push_back(compute_road_partial(som1 + (decalagex - 0.02f) * (som2 - som1) + (1 - 0.02f - decalagey) * (som3 - som2), som0 + (decalagey - 0.02f) * (som1 - som0) + (decalagex - 0.02f) * (som3 - som0), som3 + (1 - decalagex - 0.02f) * (som0 - som3) + (decalagey - 0.02f) * (som1 - som0), som2 + (1 - 0.02f - decalagex) * (som1 - som2) + (1 - decalagey - 0.02f) * (som3 - som2), true));
    }
    else if (pate[4].x == 1)
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
    else if (pate[4].x == 3)
    {
        vector<vec3> current_cube;
        vec3 som0 = pate[0];
        vec3 som1 = pate[1];
        vec3 som2 = pate[2];
        vec3 som3 = pate[3];
        float decalage = (0.3f) * rand() / (float)RAND_MAX + 0.35f;
        if ((norm(som1 - som0) < norm(som3 - som0)))
        {
            vec3 nvxs0 = som0;
            vec3 nvxs1 = som1;
            vec3 nvxs2 = som1 + (decalage - 0.01f) * (som2 - som1);
            vec3 nvxs3 = som0 + (decalage - 0.01f) * (som3 - som0);

            current_cube.push_back(nvxs0);
            current_cube.push_back(nvxs1);
            current_cube.push_back(nvxs2);
            current_cube.push_back(nvxs3);
            current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
            res.push_back(current_cube);

            current_cube.clear();

            nvxs0 = som3 + (1 - decalage - 0.01f) * (som0 - som3);
            nvxs1 = som2 + (1 - decalage - 0.01f) * (som1 - som2);
            nvxs2 = som2;
            nvxs3 = som3;

            current_cube.push_back(nvxs0);
            current_cube.push_back(nvxs1);
            current_cube.push_back(nvxs2);
            current_cube.push_back(nvxs3);
            current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
            res.push_back(current_cube);

            roads.push_back(compute_road_partial(som0 + (decalage - 0.01f) * (som3 - som0), som3 + (1 - decalage - 0.01f) * (som0 - som3), som2 + (1 - decalage - 0.01f) * (som1 - som2), som1 + (decalage - 0.01f) * (som2 - som1), false));

            current_cube.clear();
        }
        else
        {
            vec3 nvxs0 = som0;
            vec3 nvxs1 = som0 + (decalage - 0.01f) * (som1 - som0);
            vec3 nvxs2 = som3 + (decalage - 0.01f) * (som3 - som2);
            vec3 nvxs3 = som3;

            current_cube.push_back(nvxs0);
            current_cube.push_back(nvxs1);
            current_cube.push_back(nvxs2);
            current_cube.push_back(nvxs3);
            current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
            res.push_back(current_cube);

            current_cube.clear();

            nvxs0 = som1 + (1 - decalage - 0.01f) * (som0 - som1);
            nvxs1 = som1;
            nvxs2 = som2;
            nvxs3 = som2 + (1 - decalage - 0.01f) * (som3 - som2);

            current_cube.push_back(nvxs0);
            current_cube.push_back(nvxs1);
            current_cube.push_back(nvxs2);
            current_cube.push_back(nvxs3);
            current_cube.push_back(random_divider() + vec3(0, pate[4].y, 0));
            res.push_back(current_cube);

            roads.push_back(compute_road_partial(som0 + (decalage - 0.01f) * (som1 - som0), som3 + (decalage - 0.01f) * (som3 - som2), som2 + (1 - decalage - 0.01f) * (som3 - som2), som1 + (1 - decalage - 0.01f) * (som0 - som1), false));

            current_cube.clear();
        }
    }

    return res;
}

void city::merge_pat(vector<vector<vec3>> &base, vector<vector<vec3>> &to_add)
{
    for (size_t i = 0; i < to_add.size(); i++) base.push_back(to_add[i]);
}

void city::add_tree_positions(vector<tree_located> & to_add)
{
    for (size_t i = 0; i < to_add.size(); i++) trees.push_back(to_add[i]);
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
    float haut = (h_max - h_min) * rand() / (float)RAND_MAX + h_max;
    float scale = (0.15f) * rand() / (float)RAND_MAX + 0.85;
    float rotate = (0.1f) * rand() / (float)RAND_MAX - 0.05;
    float coloral = rand() / (float)RAND_MAX;
    mesh bat;
    vec3 som00 = coords[0];
    vec3 som01 = coords[1];
    vec3 som02 = coords[2];
    vec3 som03 = coords[3];
    vec3 som0 = (scale * som00 + (1 - scale) * som02);
    vec3 som1 = (scale * som01 + (1 - scale) * som03);
    vec3 som2 = (scale * som02 + (1 - scale) * som00);
    vec3 som3 = (scale * som03 + (1 - scale) * som01);
    vec3 up = vec3(0, 0, haut);
    if (prob < 0.7f)
    {
        mesh batim;
        mesh fenetr;
        vec3 nvxs0 = (rotate * som0 + (1 - rotate) * som1);
        vec3 nvxs1 = (rotate * som1 + (1 - rotate) * som2);
        vec3 nvxs2 = (rotate * som2 + (1 - rotate) * som3);
        vec3 nvxs3 = (rotate * som3 + (1 - rotate) * som0);
        batim.push_back(mesh_primitive_cubic_grid(nvxs0, nvxs3, nvxs2, nvxs1, nvxs0 + up, nvxs3 + up, nvxs2 + up, nvxs1 + up, 4, 4, 4));

        fenetr.push_back(compute_windows_on_cube(nvxs0, nvxs3, nvxs2, nvxs1, nvxs0 + up, nvxs3 + up, nvxs2 + up, nvxs1 + up));
        vec3 nvvxs0 = (3 * nvxs0 + nvxs2) / 4;
        vec3 nvvxs1 = (3 * nvxs1 + nvxs3) / 4;
        vec3 nvvxs2 = (nvxs0 + 3 * nvxs2) / 4;
        vec3 nvvxs3 = (nvxs1 + 3 * nvxs3) / 4;
        batim.push_back(mesh_primitive_cubic_grid(nvvxs0 + up, nvvxs3 + up, nvvxs2 + up, nvvxs1 + up, nvvxs0 + up + up, nvvxs3 + up + up, nvvxs2 + up + up, nvvxs1 + up + up, 4, 4, 4));
        fenetr.push_back(compute_windows_on_cube(nvvxs0 + up, nvvxs3 + up, nvvxs2 + up, nvvxs1 + up, nvvxs0 + up + up, nvvxs3 + up + up, nvvxs2 + up + up, nvvxs1 + up + up));
        nvvxs0 = (3 * nvxs0 + 2 * nvxs2) / 5;
        nvvxs1 = (3 * nvxs1 + 2 * nvxs3) / 5;
        nvvxs2 = (2 * nvxs0 + 3 * nvxs2) / 5;
        nvvxs3 = (2 * nvxs1 + 3 * nvxs3) / 5;
        batim.push_back(mesh_primitive_cubic_grid(nvvxs0 + up + up, nvvxs3 + up + up, nvvxs2 + up + up, nvvxs1 + up + up, nvvxs0 + up + up + up, nvvxs3 + up + up + up, nvvxs2 + up + up + up, nvvxs1 + up + up + up, 2, 2, 2));
        fenetr.push_back(compute_windows_on_cube(nvvxs0 + up + up, nvvxs3 + up + up, nvvxs2 + up + up, nvvxs1 + up + up, nvvxs0 + up + up + up, nvvxs3 + up + up + up, nvvxs2 + up + up + up, nvvxs1 + up + up + up));
        if (coloral < 0.5f)
        {
            batim.color.fill(vec3(245, 245, 220) / 255.0f);
        }
        else
        {
            batim.color.fill(vec3(206, 206, 206) / 255.0f); // couleur grise
        }
        bat.push_back(batim);
        bat.push_back(fenetr);
    }
    else if (prob < 1.0f)
    {
        bat.push_back(mesh_primitive_cubic_grid(som0, som3, som2, som1, som0 + 2 * up, som3 + 2 * up, som2 + 2 * up, som1 + 2 * up, 4, 4, 4));
        for (auto i = 0; i < bat.color.size(); i++)
        {
            bat.color[i] = vec3(250, 240, 197) / 255.0f;
        }
        bat.push_back(compute_windows_on_cube(som0, som3, som2, som1, som0 + 2 * up, som3 + 2 * up, som2 + 2 * up, som1 + 2 * up));
    }
    return bat;
}

mesh city::compute_garden(vector<vec3> coords)
{
    int Nu = 15;
    int Nv = 15;
    // Pour compute les arbres, generer un vecteur de positions d'arbres et de hauteur, qui sera ensuite executé au moment du draw
    mesh sol = mesh_primitive_grid(coords[0], coords[3], coords[2], coords[1], Nu, Nv);
    sol.uv.clear();
    for (size_t ku = 0; ku < size_t(Nu); ++ku)
    {
        for (size_t kv = 0; kv < size_t(Nv); ++kv)
        {
            sol.uv.push_back(vec2(ku, kv));
        }
    }
    vec3 p00 = coords[0];
    vec3 p10 = coords[3];
    vec3 p11 = coords[2];
    vec3 p01 = coords[1];
    float aire = norm(p10 - p00) * norm(p01 - p00);
    int N = 0;
    if (aire < 1)
        N = 1;
    else if (aire < 4)
        N = 3;
    else if (aire < 20)
        N = 6;
    else if (aire < 50)
        N = 10;
    else if (aire < 90)
        N = 16;
    else
        N = 60;
    vector<tree_located> result;
    for (int i = 0; i < N; i++)
    {
        float u = rand() / (float)RAND_MAX;
        float v = rand() / (float)RAND_MAX;
        float w = rand() / (float)RAND_MAX;
        float z = rand() / (float)RAND_MAX;
        vec3 p = (u * p00 + w * p01 + v * p11 + z * p10) / (u + v + w + z);

        bool enlv = false;
        float type;
        vec3 q;
        for (tree_located tree : result)
        {
            q = tree.position;
            if (((q.x - p.x) * (q.x - p.x) + (q.y - p.y) * (q.y - p.y)) < 0.15) // Passer environ à 0.20 pour eviter les collisions entre les tree
                enlv = true;
        }
        if (!enlv)
        {

            result.push_back({ TreeGenerator::random_tree_type(), p});
        }
    }
    add_tree_positions(result);
    return sol;
}

void city::create_city(vector<vec3> coords)
{
    if (coords[4].x == 0) 
        batiments.push_back(compute_batiment(coords));
    else if (coords[4].x == 1)
        parcs.push_back(compute_garden(coords));
    else
        batiments.push_back(compute_batiment(coords));
}

mesh city::compute_windows_on_cube(vec3 const &p000, vec3 const &p100, vec3 const &p110, vec3 const &p010, vec3 const &p001, vec3 const &p101, vec3 const &p111, vec3 const &p011)
{
    mesh res;
    res.push_back(compute_windows_on_quadrangle(p000, p001, p101, p100));
    res.push_back(compute_windows_on_quadrangle(p100, p101, p111, p110));
    res.push_back(compute_windows_on_quadrangle(p110, p111, p101, p010));
    res.push_back(compute_windows_on_quadrangle(p010, p011, p001, p000));
    return res;
}

mesh city::compute_windows_on_quadrangle(vec3 const &p00, vec3 const &p10, vec3 const &p11, vec3 const &p01)
{
    float height = 0.05f;
    float length = 0.1f;
    float decalhoriz = 0.7f * length;
    float decalvert = 0.7f * height;
    vec3 xloc = normalize(p10 - p00);
    vec3 yloc = normalize(p01 - p00);
    vec3 decalnormal = cross(yloc, xloc);
    vec3 current_hor = p00 + eps * decalnormal;
    vec3 current_vert = p00 + eps * decalnormal;
    mesh res;
    float imax = floor(norm(p10 - p00) / (length + decalhoriz));
    float jmax = floor(norm(p01 - p00) / (height + decalvert));
    for (int j = 0; j < jmax; j++)
    {
        current_vert = current_vert + decalvert * yloc;
        for (int i = 0; i < imax; i++)
        {
            current_hor += decalhoriz * xloc;
            vec3 currentp00 = current_hor + current_vert - p00;
            mesh fenetre = mesh_primitive_quadrangle(currentp00, currentp00 + length * xloc, currentp00 + length * xloc + height * yloc, currentp00 + height * yloc);
            float randf = rand() / (float)RAND_MAX;
            if (randf < 0.8f)
                fenetre.color.fill(vec3(240, 255, 255) / 255.0f);
            else
                fenetre.color.fill(vec3(239, 239, 239) / 255.0f);
            res.push_back(fenetre);
            current_hor += length * xloc;
        }
        current_hor = p00;
        current_vert = current_vert + height * yloc;
    }
    return res;
}

mesh city::compute_road_partial(vec3 const &p00, vec3 const &p10, vec3 const &p11, vec3 const &p01, bool const &isspecial)
{
    mesh roa = mesh_primitive_quadrangle(p00, p10, p11, p01);
    roa.color.fill(vec3(0, 0, 0));
    mesh circul;

    if (!isspecial)
    {
        if (norm(p10 - p00) > norm(p01 - p00))
        {
            float normal = norm(p10 - p00);
            float length = 0.06f * norm(p01 - p00);
            float height = 0.02f * normal;
            float decalvert = 0.2f * height;
            float jmax = floor(normal / (height + decalvert));
            vec3 xloc = normalize((p10 + p11) / 2 - (p01 + p00) / 2);
            vec3 yloc = normalize(p01 - p00);
            vec3 decalnormal = cross(xloc, yloc);
            vec3 current_hor = p00 + 0.497f * (p01 - p00) + 0.005f * decalnormal;
            vec3 current_vert = p00 + 0.005f * decalnormal;

            for (auto j = 0; j < jmax; j++)
            {
                current_vert += decalvert * xloc;
                vec3 currentp00 = current_hor + current_vert - p00;
                circul.push_back(mesh_primitive_quadrangle(currentp00, currentp00 + height * xloc, currentp00 + length * yloc + height * xloc, currentp00 + length * yloc));
                current_vert += height * xloc;
            }
        }
        else
        {
            float normal = norm(p01 - p00);
            float length = 0.06f * norm(p10 - p00);
            float height = 0.02f * normal;
            float decalvert = 0.2f * height;
            float jmax = floor(normal / (height + decalvert));
            vec3 xloc = normalize((p01 + p11) / 2 - (p10 + p00) / 2);
            vec3 yloc = normalize(p10 - p00);
            vec3 decalnormal = cross(yloc, xloc);
            vec3 current_hor = p00 + 0.497f * (p10 - p00) + 0.005f * decalnormal;
            vec3 current_vert = p00 + 0.005f * decalnormal;

            for (auto j = 0; j < jmax; j++)
            {
                current_vert += decalvert * xloc;
                vec3 currentp00 = current_hor + current_vert - p00;
                circul.push_back(mesh_primitive_quadrangle(currentp00, currentp00 + height * xloc, currentp00 + length * yloc + height * xloc, currentp00 + length * yloc));
                current_vert += height * xloc;
            }
        }
        circul.color.fill(vec3(1.0, 1.0, 1.0));
        roa.push_back(circul);
    }
    else
    {
        vec3 xloc = normalize(p01 - p00);
        vec3 yloc = normalize(p10 - p00);
        float epaisseur = 0.05f * norm(p01 - p00);
        vec3 decalnormal = cross(yloc, xloc);
        vec3 p00s = 0.005f * decalnormal;
        circul.push_back(mesh_primitive_quadrangle(p00 + p00s, p00 + p00s + epaisseur * yloc, p01 + p00s + epaisseur * yloc, p01 + p00s));
        circul.push_back(mesh_primitive_quadrangle(p00 + p00s, p10 + p00s, p10 + p00s + epaisseur * xloc, p00 + p00s + epaisseur * xloc));
        circul.push_back(mesh_primitive_quadrangle(p10 + p00s - epaisseur * yloc, p10 + p00s, p11 + p00s, p11 + p00s - epaisseur * yloc));
        circul.push_back(mesh_primitive_quadrangle(p11 + p00s - epaisseur * xloc, p11 + p00s, p01 + p00s, p01 + p00s - epaisseur * xloc));
        circul.color.fill(vec3(1.0, 1.0, 1.0));
        roa.push_back(circul);
    }
    return roa;
}

float city::get_altitude(vec2 const &new_position_in_plane)
{
    if (user_reference->sneak)
        return player_height / 2;
    else
        return player_height;
}
