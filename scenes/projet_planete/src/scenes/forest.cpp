#include "forest.hpp"

#include <list>
#include <set>

using namespace vcl;

forest::forest(user_parameters *user, std::function<void(scene_type)> _swap_function) : environement(user, _swap_function)
{
    x_min = -5.0f;
    x_max = 5.0f;
    y_min = -5.0f;
    y_max = 5.0f;

    // Configuration des visuels
    set_terrain();
    set_skybox();
    set_sun();
    set_grass();
    set_trees();
    set_sand();

    /* Initialise la caméra*/
    camera_m.manipulator_set_altitude(get_altitude(camera_m.position().xy()));

    /* Initialise les arbres*/
    GLuint tree_shader = scene_visual::get_shader(shader_type::TREE);
    tree_cool.initTree(tree_type::REAL_2, tree_shader);
    tree_real.initTree(tree_type::REAL_1, tree_shader);
    tree_classic.initTree(tree_type::CLASSIC, tree_shader);
    tree_cool.resize(0.5f);
    tree_real.resize(0.5f);
    tree_classic.resize(0.5f);

    // Tests
    vec2 position = 0.8 * vec2(x_min, x_min);
    std::cout << profile(position) << std::endl;
}

void forest::display_visual()
{
    user_reference->timer.update();
    float const time = user_reference->timer.t;
    glClearColor(0.256f, 0.256f, 0.256f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (m_activated)
        light = camera_m.position();
    else
        light = camera_c.position();

    /* Shaders */
    GLuint normal_shader = get_shader(shader_type::NORMAL);
    GLuint sun_shader = get_shader(shader_type::SUN);

    GLuint tree_shader = get_shader(shader_type::TREE);

    GLuint forest_shader = get_shader(shader_type::FOREST);

    glUseProgram(normal_shader);
    opengl_uniform(normal_shader, "projection", projection);
    if (m_activated)
        opengl_uniform(normal_shader, "view", camera_m.matrix_view());
    else
        opengl_uniform(normal_shader, "view", camera_c.matrix_view());
    opengl_uniform(normal_shader, "light", light);

    glUseProgram(sun_shader);
    opengl_uniform(sun_shader, "projection", projection);
    if (m_activated)
        opengl_uniform(sun_shader, "view", camera_m.matrix_view());
    else
        opengl_uniform(sun_shader, "view", camera_c.matrix_view());
    opengl_uniform(sun_shader, "light", light);

    glUseProgram(tree_shader);
    opengl_uniform(tree_shader, "projection", projection);
    if (m_activated)
        opengl_uniform(tree_shader, "view", camera_m.matrix_view());
    else
        opengl_uniform(tree_shader, "view", camera_c.matrix_view());
    opengl_uniform(tree_shader, "light", light);

    glUseProgram(forest_shader);
    opengl_uniform(forest_shader, "projection", projection);
    if (m_activated)
        opengl_uniform(forest_shader, "view", camera_m.matrix_view());
    else
        opengl_uniform(forest_shader, "view", camera_c.matrix_view());
    opengl_uniform(forest_shader, "light", light);

    draw(grass_sol, this);

    draw(sand_visual, this);

    for (tree_located tree : trees)
    {
        switch (tree.type)
        {
        case tree_type::COOL:
            tree_cool.translate(tree.position);
            draw(tree_cool.dtrunk, this);
            if (tree_cool.hasleaves)
                draw(tree_cool.dleaves, this);
            if (user_reference->draw_wireframe)
            {
                draw_wireframe(tree_cool.dtrunk, this);
                if (tree_cool.hasleaves)
                    draw_wireframe(tree_cool.dleaves, this);
            }
            break;
        case tree_type::CLASSIC:
            tree_classic.translate(tree.position);
            draw(tree_real.dtrunk, this);
            if (tree_real.hasleaves)
                draw(tree_real.dleaves, this);
            if (user_reference->draw_wireframe)
            {
                draw_wireframe(tree_real.dtrunk, this);
                if (tree_real.hasleaves)
                    draw_wireframe(tree_real.dleaves, this);
            }
            break;
        case tree_type::REAL_1:
            tree_real.translate(tree.position);
            draw(tree_real.dtrunk, this);
            if (tree_real.hasleaves)
                draw(tree_real.dleaves, this);
            if (user_reference->draw_wireframe)
            {
                draw_wireframe(tree_real.dtrunk, this);
                if (tree_real.hasleaves)
                    draw_wireframe(tree_real.dleaves, this);
            }
            break;
        case tree_type::REAL_2:
            tree_cool.translate(tree.position);
            draw(tree_cool.dtrunk, this);
            if (tree_cool.hasleaves)
                draw(tree_cool.dleaves, this);
            if (user_reference->draw_wireframe)
            {
                draw_wireframe(tree_cool.dtrunk, this);
                if (tree_cool.hasleaves)
                    draw_wireframe(tree_cool.dleaves, this);
            }
            break;
        }
    }

    opengl_check;
    draw(visual, this);

    if (user_reference->draw_wireframe)
        draw_wireframe(visual, this);
}

void forest::update_visual()
{
    super::update_visual();
}

void forest::display_interface()
{
    super::display_interface();
}

void forest::set_terrain()
{
    vec3 p_1 = forest_proportion * vec3(x_min, y_min, 0.0);
    vec3 p_2 = forest_proportion * vec3(x_max, y_min, 0.0);
    vec3 p_3 = forest_proportion * vec3(x_max, y_max, 0.0);
    vec3 p_4 = forest_proportion * vec3(x_min, y_max, 0.0);

    int N_terrain = forest_proportion * N;
    mesh = mesh_primitive_grid(p_1, p_2, p_3, p_4, N_terrain, N_terrain);
    for (int i = 0; i < mesh.position.size() ; i++)
    {
        mesh.position[i].z = get_altitude(mesh.position[i].xy());
        mesh.uv[i] = mesh.position[i].xy();
    }
    visual = mesh_drawable(mesh, get_shader(shader_type::NORMAL));
    visual.texture = scene_visual::get_texture(texture_type::GRASS);
}

void forest::set_skybox()
{
    skybox.init_skybox(vec3(0, 0, 0), skybox_radius, skybox_type::SUNDOWN, get_shader(shader_type::NORMAL));
}

void forest::set_sun()
{
    sun_visual = mesh_drawable(mesh_primitive_sphere(sun_radius), get_shader(shader_type::SUN));
    sun_visual.shading.color = vec3(1.0, 1.0, 0.0);
}

void forest::set_sand()
{
    vcl::mesh sand, side;
    int N_small = (1 - forest_proportion) / 2 * N;
    int N_big = N;
    vec3 inner_1 = forest_proportion * vec3(x_min, y_min, 0.0);
    vec3 inner_2 = forest_proportion * vec3(x_max, y_min, 0.0);
    vec3 inner_3 = forest_proportion * vec3(x_max, y_max, 0.0);
    vec3 inner_4 = forest_proportion * vec3(x_min, y_max, 0.0);
    vec3 outer_1 = vec3(x_min, y_min, 0.0);
    vec3 outer_2 = vec3(x_max, y_min, 0.0);
    vec3 outer_3 = vec3(x_max, y_max, 0.0);
    vec3 outer_4 = vec3(x_min, y_max, 0.0);
    sand.push_back(mesh_primitive_grid(inner_1, inner_2, outer_2, outer_1, N_big, N_small));
    sand.push_back(mesh_primitive_grid(inner_2, inner_3, outer_3, outer_2, N_big, N_small));
    sand.push_back(mesh_primitive_grid(inner_3, inner_4, outer_4, outer_3, N_big, N_small));
    sand.push_back(mesh_primitive_grid(inner_4, inner_1, outer_1, outer_4, N_big, N_small));

    for (int i = 0; i < sand.position.size(); i++)
    {
        sand.position[i].z = get_altitude(sand.position[i].xy());
    }
    for (int i = 0; i < sand.position.size(); i++)
    {
        sand.uv[i].x = 2 * sand.position[i].x;
        sand.uv[i].y = 2 * sand.position[i].y;
    }
    GLuint normal_shader = get_shader(shader_type::NORMAL);
    sand_visual = mesh_drawable(sand, normal_shader);
    sand_visual.texture = get_texture(texture_type::SAND);
}

float forest::profile(vec2 const &position_in_plane)
{
    float z_min = -0.2;
    float z_max = 0.2;
    float coord_x = std::max(position_in_plane.x / x_max, position_in_plane.x / x_min);
    float coord_y = std::max(position_in_plane.y / y_max, position_in_plane.y / y_min);
    float coord = std::max(coord_x, coord_y);
    if (coord < profile_transition_down)
        return z_max;
    else if (coord < profile_transition_up)
    {
        float t = 0.5 * (1 + cos(PI / (profile_transition_up - profile_transition_down) * (coord - profile_transition_down)));
        return t * z_max + (1 - t) * z_min;
    }
    else
        return z_min;
}

float forest::get_altitude(vec2 const& position_on_terrain) 
{
    return profile(position_on_terrain) + parameters.height * noise_perlin(position_on_terrain, parameters.octaves, parameters.persistency, parameters.frequency_gain);
}

void forest::set_grass()
{
    for (int i = 0; i < nb_particles; i++)
    {
        vec3 xloc = vec3(0.05, 0, 0);
        vec3 yloc = vec3(0, 0.05, 0);
        vec3 decal_vert = vec3(0, 0, 0.05);
        float alpha = 2 * pi * rand_interval();
        vec3 decal_hor = cos(alpha) * xloc + sin(alpha) * yloc;
        vec3 pos = forest_proportion * vec3((x_max - x_min) * (rand_interval() - 0.5f), (y_max - y_min) * (rand_interval() - 0.5f), 0.0);
        pos.z = get_altitude(pos.xy());
        grass.push_back(mesh_primitive_quadrangle(pos, pos + decal_hor, pos + decal_hor + decal_vert, pos + decal_vert));
        float row = rand() % 8;
        float column = rand() % 8;
        buff_vecgrass.push_back(vec3(row, column, 0));
        buff_vecgrass.push_back(vec3(row, column, 0));
        buff_vecgrass.push_back(vec3(row, column, 0));
        buff_vecgrass.push_back(vec3(row, column, 0));
        buff_floatgrass.push_back(8.0);
        buff_floatgrass.push_back(8.0);
        buff_floatgrass.push_back(8.0);
        buff_floatgrass.push_back(8.0);
    }

    grass_sol = mesh_float_drawable(grass, buff_floatgrass, buff_vecgrass, scene_visual::get_shader(shader_type::FOREST), scene_visual::get_texture(texture_type::GRASS_ATLAS));
}

void forest::set_trees()
{
    trees.resize(nb_tree);
    std::vector<vcl::vec3> pos = generate_positions_on_terrain(nb_tree);
    for (int i = 0; i < nb_tree; i++)
    {
        trees[i] = {TreeGenerator::random_tree_type(), pos[i]};
    }
}

std::vector<vcl::vec3> forest::generate_positions_on_terrain(int N)
{
    std::vector<vcl::vec3> res;
    int nb_add = 0;
    while (nb_add < N)
    {
        float u = (x_max - x_min) * (rand_interval() - 0.5f);
        float v = (y_max - y_min) * (rand_interval() - 0.5f);
        vec3 p = forest_proportion * vec3(u, v, 0);
        p.z = get_altitude(p.xy());
        bool enlv = false;
        for (vec3 q : res)
        {
            if (((q.x - p.x) * (q.x - p.x) + (q.y - p.y) * (q.y - p.y)) < 0.20) // Passer environ à 0.20 pour eviter les collisions entre les tree
            {
                enlv = true;
            }
        }
        if (!enlv)
        {
            res.push_back(p);
            nb_add++;
        }
    }
    return res;
}
