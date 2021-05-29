#include "forest.hpp"

#include <list>
#include <set>

using namespace vcl;

forest::forest(user_parameters *user, std::function<void(scene_type)> _swap_function) : environement(user, _swap_function)
{

    // Configuration des visuels
    set_terrain();
    set_skybox();
    set_sun();
    set_grass();
    set_trees();

    /* Initialise la caméra*/
    camera_m.manipulator_set_altitude(get_altitude(camera_m.position().xy()));

    /* Initialise les arbres*/
    GLuint tree_shader = scene_visual::get_shader(shader_type::TREE);
    tree_cool.initTree(tree_type::REAL_2, tree_shader);
    tree_real.initTree(tree_type::REAL_1, tree_shader);
    tree_classic.initTree(tree_type::CLASSIC, tree_shader);
}

void forest::display_visual()
{
    user_reference->timer.update();
    float const time = user_reference->timer.t;
    if (m_activated)
        light = camera_m.position();
    else
        light = camera_c.position();

    /* Shaders */
    GLuint normal_shader = get_shader(shader_type::NORMAL);
    GLuint sun_shader = get_shader(shader_type::SUN);
    GLuint partic_shader = get_shader(shader_type::PARTICLE);
    GLuint tree_shader = get_shader(shader_type::TREE);

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

    glUseProgram(partic_shader);
    opengl_uniform(partic_shader, "projection", projection);
    if (m_activated)
        opengl_uniform(partic_shader, "view", camera_m.matrix_view());
    else
        opengl_uniform(partic_shader, "view", camera_c.matrix_view());

    glUseProgram(tree_shader);
    opengl_uniform(tree_shader, "projection", projection);
    if (m_activated)
        opengl_uniform(tree_shader, "view", camera_m.matrix_view());
    else
        opengl_uniform(tree_shader, "view", camera_c.matrix_view());
    opengl_uniform(tree_shader, "light", light);

    /* Particules*/
    if (m_activated)
        grass.updateParticles(camera_m.position());
    else
        grass.updateParticles(camera_c.position());
    grass.updateShadVbos(this);

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
    int Nu = 40;
    int Nv = 40;
    mesh = mesh_primitive_grid({x_min, y_min, 0}, {x_max, y_min, 0}, {x_max, y_max, 0}, {x_min, y_max, 0}, Nu, Nv);
    for (int i = 0; i < Nu; i++)
    {
        for (int j = 0; j < Nv; j++)
        {
            const float u = i / (Nu - 1.0f);
            const float v = j / (Nv - 1.0f);
            float x = 2 * (x_max - x_min) * (u - 0.5f);
            float y = 2 * (y_max - y_min) * (v - 0.5f);

            mesh.position[j + Nv * i].z += parameters.height * noise_perlin(mesh.position[j + Nv * i], parameters.octaves, parameters.persistency, parameters.frequency_gain);
            mesh.uv[j + Nv * i] = vec2(x, y);
        }
    }
    visual = mesh_drawable(mesh, get_shader(shader_type::NORMAL));
    visual.texture = scene_visual::get_texture(texture_type::GRASS);
}

void forest::set_skybox()
{
    skybox.init_skybox(vec3(0, 0, 0), 10, skybox_type::SUNDOWN, get_shader(shader_type::NORMAL));
}

void forest::set_sun()
{
    sun_visual = mesh_drawable(mesh_primitive_sphere(sun_radius), get_shader(shader_type::SUN));
    sun_visual.shading.color = vec3(1.0, 1.0, 0.0);
}

void forest::set_grass()
{
    grass = Particles(nb_particles, "grass", x_min, x_max, y_min, y_max);
    grass.initVaoVbo(scene_visual::get_shader(shader_type::PARTICLE));
}

void forest::set_trees()
{
    trees.resize(nb_tree);
    for (int i = 0; i < nb_tree; i++)
    {
        vec3 position = vec3((x_max - x_min) * (rand_interval() - 0.5f), (x_max - x_min) * (rand_interval() - 0.5f), 0);
        position.z = parameters.height * noise_perlin(position.xy(), parameters.octaves, parameters.persistency, parameters.frequency_gain);
        trees[i] = {TreeGenerator::random_tree_type(), position};
    }
}

float forest::get_altitude(vcl::vec2 const &position_in_plane)
{
    float z = parameters.height * noise_perlin(position_in_plane, parameters.octaves, parameters.persistency, parameters.frequency_gain);
    if (user_reference->sneak)
        return player_height / 2 + z;
    else
        return player_height + z;
}
