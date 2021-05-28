#include "mountain.hpp"

#include <list>
#include <set>

using namespace vcl;

mountain::mountain(user_parameters *user, std::function<void(scene_type)> _swap_function) : environement(user, _swap_function)
{

    // Configuration des visuels
    set_terrain();
    set_skybox();
    set_sun();

    // Configuration de la cam�ra

    // Configuration de la lumi�re
    light = vec3(1.0f, 1.0f, 1.0f);

    // Configuration du swap
    swap_function = _swap_function;
}

void mountain::display_visual()
{
    glClearColor(0.256f, 0.256f, 0.256f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    user_reference->timer.update();
    float const time = user_reference->timer.t;
    if (m_activated) light = camera_m.position();
    else light = camera_c.position();

    GLuint normal_shader = get_shader(shader_type::NORMAL);
    GLuint sun_shader = get_shader(shader_type::SUN);

    glUseProgram(normal_shader);
    opengl_uniform(normal_shader, "projection", projection);
    if (m_activated) opengl_uniform(normal_shader, "view", camera_m.matrix_view());
    else opengl_uniform(normal_shader, "view", camera_c.matrix_view());
    opengl_uniform(normal_shader, "light", light);

    glUseProgram(sun_shader);
    opengl_uniform(sun_shader, "projection", projection);
    if (m_activated) opengl_uniform(sun_shader, "view", camera_m.matrix_view());
    else opengl_uniform(sun_shader, "view", camera_c.matrix_view());
    opengl_uniform(sun_shader, "light", light);

    draw(visual, this);

    if (user_reference->draw_wireframe)
        draw_wireframe(visual, this);

    skybox.display_skybox(this);

    draw(sun_visual, this);
}

void mountain::update_visual()
{
    super::update_visual();
}

void mountain::display_interface()
{
    super::display_interface();
}

float mountain::get_altitude(vcl::vec2 const& position_in_plane)
{
    if (user_reference->sneak) return player_height / 2;
    return player_height;
}

void mountain::set_terrain()
{
    mesh = mesh_primitive_grid();
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            mesh.position[j].z = 0.1 * exp(-float(i) / 10 - float(j) / 10);
        }
    }

    visual = mesh_drawable(mesh, get_shader(shader_type::NORMAL));
}

void mountain::set_skybox()
{
    skybox.init_skybox(vec3(0, 0, 0), 10, skybox_type::SUNDOWN, get_shader(shader_type::NORMAL));
}

void mountain::set_sun()
{
    sun_visual = mesh_drawable(mesh_primitive_sphere(sun_radius), get_shader(shader_type::SUN));
    sun_visual.shading.color = vec3(1.0, 1.0, 0.0);
}