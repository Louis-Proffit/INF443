#include "forest.hpp"

#include <list>
#include <set>

using namespace vcl;

forest::forest(user_parameters *user, std::function<void(scene_type)> _swap_function) : scene_visual(user, _swap_function)
{

    // Configuration des visuels
    set_terrain();
    set_skybox();
    set_sun();
    set_grass();
    set_tree();

    // Configuration de la cam�ra
    camera.distance_to_center = 2.5f;
    camera.look_at({4, 3, 2}, {0, 0, 0}, {0, 0, 1});

    // Configuration de la lumi�re
    light = vec3(1.0f, 1.0f, 1.0f);

    // Configuration du swap
    swap_function = _swap_function;
}

forest::~forest() {}

void forest::display_visual()
{
    user_reference->timer.update();
    float const time = user_reference->timer.t;
    light = camera.position();

    GLuint normal_shader = open_shader("normal");
    GLuint sun_shader = open_shader("sun");
    GLuint partic_shader = open_shader("partic");

    glUseProgram(normal_shader);
    opengl_uniform(normal_shader, "projection", projection);
    opengl_uniform(normal_shader, "view", camera.matrix_view());
    opengl_uniform(normal_shader, "light", light);

    glUseProgram(sun_shader);
    opengl_uniform(sun_shader, "projection", projection);
    opengl_uniform(sun_shader, "view", camera.matrix_view());
    opengl_uniform(sun_shader, "light", light);

    glUseProgram(partic_shader);
    opengl_uniform(partic_shader, "projection", projection);
    opengl_uniform(partic_shader, "view", camera.matrix_view());
    //opengl_uniform(partic_shader, "light", light);
    grass.updateParticles(camera.position());
    grass.updateShadVbos(this);

    tree.draw_tree(this);
    opengl_check;
    draw(visual, this);

    if (user_reference->draw_wireframe)
        draw_wireframe(visual, this);

    //skybox.display_skybox(this);

    //draw(sun_visual, this);
}

void forest::update_visual()
{
    vec2 const &p0 = user_reference->mouse_prev;
    vec2 const &p1 = user_reference->mouse_curr;

    if (!user_reference->cursor_on_gui && !user_reference->state.key_shift)
    {
        if (user_reference->state.mouse_click_left && !user_reference->state.key_ctrl)
            camera.manipulator_rotate_trackball(p0, p1);
        if (user_reference->state.mouse_click_left && user_reference->state.key_ctrl)
            camera.manipulator_translate_in_plane(p1 - p0);
        if (user_reference->state.mouse_click_right)
            camera.manipulator_scale_distance_to_center((p1 - p0).y);
    }

    user_reference->mouse_prev = p1;
}

void forest::display_interface()
{
    ImGui::Checkbox("Frame", &user_reference->display_frame);
    ImGui::Checkbox("Wireframe", &user_reference->draw_wireframe);
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

            // mesh.position[j + Nv * i].z = 3 * exp(-float(i) / 10 - float(j) / 10);
            mesh.position[j + Nv * i].z += parameters.height * noise_perlin(mesh.position[j + Nv * i], parameters.octaves, parameters.persistency, parameters.frequency_gain);
            mesh.uv[j + Nv * i] = vec2(x, y);
        }
    }
    visual = mesh_drawable(mesh, open_shader("normal"));
    visual.texture = opengl_texture_to_gpu(image_load_png("../assets/textures/grass/grass-ground.png"), GL_REPEAT /**GL_TEXTURE_WRAP_S*/, GL_REPEAT /**GL_TEXTURE_WRAP_T*/);
}

void forest::set_skybox()
{
    skybox.init_skybox(vec3(0, 0, 0), 10, "sundown", open_shader("normal"));
}

void forest::set_sun()
{
    sun_visual = mesh_drawable(mesh_primitive_sphere(sun_radius), open_shader("sun"));
    sun_visual.shading.color = vec3(1.0, 1.0, 0.0);
}

void forest::set_grass()
{
    grass = *(new ParticleS(2000, "grass"));
    grass.initVaoVbo();
}

void forest::set_tree()
{
    tree.initTree("Realtree_1");
}