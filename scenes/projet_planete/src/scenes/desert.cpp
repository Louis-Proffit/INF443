#include "desert.hpp"

#include <list>
#include <set>

using namespace vcl;

desert::desert(user_parameters* user, std::function<void(scene_type)> _swap_function) : scene_visual(user, _swap_function)
{

    // Configuration des visuels
    set_terrain();
    set_skybox();
    set_sun();

    // Configuration de la cam�ra
    camera.position_camera = vec3(0.5, 0.5, 0.1);
    /*camera.orientation_camera = rotation(vec3(0, 0, 1), PI / 4);*/

    // Configuration de la lumi�re
    light = vec3(1.0f, 1.0f, 1.0f);

    // Configuration du swap
    swap_function = _swap_function;

    // Configuration de l'utilisateur
    user_reference = user;
}

desert::~desert() {}

void desert::display_visual()
{
    user_reference->timer.update();
    float const time = user_reference->timer.t;
    light = camera.position();

    GLuint normal_shader = open_shader("normal");
    GLuint sun_shader = open_shader("sun");

    glUseProgram(normal_shader);
    opengl_uniform(normal_shader, "projection", projection);
    opengl_uniform(normal_shader, "view", camera.matrix_view());
    opengl_uniform(normal_shader, "light", light);

    glUseProgram(sun_shader);
    opengl_uniform(sun_shader, "projection", projection);
    opengl_uniform(sun_shader, "view", camera.matrix_view());
    opengl_uniform(sun_shader, "light", light);


    draw(visual, this);

    if (user_reference->draw_wireframe) draw_wireframe(visual, this);

    skybox.display_skybox(this);

    draw(sun_visual, this);
}

void desert::update_visual()
{
    vec2 const& p0 = user_reference->mouse_prev;
    vec2 const& p1 = user_reference->mouse_curr;

    vec2 dp(0, 0);

    if (!user_reference->cursor_on_gui && !user_reference->state.key_shift) {
        if (user_reference->state.mouse_click_left && !user_reference->state.key_ctrl) {
            camera.manipulator_rotate_2_axis(p1.y - p0.y, p1.x - p0.x);
        }
        if (user_reference->state.key_up) dp.y += 1;
        if (user_reference->state.key_down) dp.y -= 1;
        if (user_reference->state.key_left) dp.x -= 1;
        if (user_reference->state.key_right) dp.x += 1;
        dp *= player_speed;
        camera.manipulator_translate_in_plane(dp);
    }

    user_reference->mouse_prev = p1;
}


void desert::display_interface()
{
    ImGui::Checkbox("Frame", &user_reference->display_frame);
    ImGui::Checkbox("Wireframe", &user_reference->draw_wireframe);
}

void desert::set_terrain()
{
    mesh = mesh_primitive_grid();
    for (int i = 0 ; i < 10; i++) {
        for (int j = 0 ; j < 10; j++) {
            mesh.position[i * 10 + j].z = exp(-float(i) / 10 - float(j) / 10);
        }
    }
    GLuint normal_shader = open_shader("normal");

    visual = mesh_drawable(mesh, open_shader("normal"));
}

void desert::set_skybox() {
    skybox.init_skybox(vec3(0, 0, 0), 10, "sundown", open_shader("normal"));
}

void desert::set_sun()
{
    sun_visual = mesh_drawable(mesh_primitive_sphere(sun_radius), open_shader("sun"));
    sun_visual.shading.color = vec3(1.0, 1.0, 0.0);
}