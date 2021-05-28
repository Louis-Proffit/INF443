#include "desert.hpp"

#include <list>
#include <set>

using namespace vcl;

desert::desert(user_parameters *user, std::function<void(scene_type)> _swap_function) : environement(user, _swap_function)
{

    // Configuration des visuels
    set_terrain();
    set_skybox();
    set_sun();

    // Configuration de la lumi�re
    light = sun_visual.transform.translate;
}

void desert::display_visual()
{
    glClearColor(0.256f, 0.256f, 0.256f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    user_reference->timer.update();
    float const time = user_reference->timer.t;
    light = sun_visual.transform.translate;

    GLuint normal_shader = open_shader(shader_type::NORMAL);
    GLuint sun_shader = open_shader(shader_type::SUN);

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
    ;
    opengl_uniform(sun_shader, "light", light);

    draw(terrain_visual, this);
    if (user_reference->draw_wireframe)
        draw_wireframe(terrain_visual, this);
    /*draw(sun_visual, this);
    if (user_reference->draw_wireframe) draw_wireframe(sun_visual, this);*/
}

void desert::update_visual()
{
    super::update_visual();

    if (height_updated)
    {
        for (int i = 0; i < parameters.rows; i++)
        {
            for (int j = 0; j < parameters.columns; j++)
                terrain_mesh.position[parameters.columns * i + j].z = height_data[i][j] * horizontal_scale + profile(terrain_mesh.position[parameters.columns * i + j].xy());
        }
        terrain_visual.update_position(terrain_mesh.position);
        height_updated = false;
    }
}

void desert::display_interface()
{
    height_updated = ImGui::SliderFloat("Echelle horizontale", &horizontal_scale, 0, 3.0f, "%.3f", 2);
    super::display_interface();
}

void desert::set_terrain()
{
    parameters = heightmap_parameters{0, 0, x_min, y_min, x_max, y_max};
    horizontal_scale = 1.0f;
    height_data = generateFileHeightData("assets/heightmaps/desert.png", horizontal_scale);
    terrain_mesh = createFromHeightData(height_data, parameters);
    for (int i = 0; i < terrain_mesh.position.size(); i++) terrain_mesh.position[i].z += profile(terrain_mesh.position[i].xy());
    terrain_visual = mesh_drawable(terrain_mesh, open_shader(shader_type::NORMAL));

    image_raw texture = image_load_png("assets/textures/sand_texture.png");
    GLuint texture_id = opengl_texture_to_gpu(texture, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    terrain_visual.texture = texture_id;
}

void desert::set_skybox()
{
    skybox.init_skybox(vec3(0, 0, 0), x_max - x_min + y_max - y_min, "desert", open_shader(shader_type::NORMAL));
}

void desert::set_sun()
{
    sun_mesh = mesh_primitive_sphere(sun_radius);
    sun_mesh.flip_connectivity();
    sun_visual = mesh_drawable(sun_mesh, open_shader(shader_type::SUN));
    sun_visual.shading.color = vec3(1.0, 1.0, 0.0);
}

float desert::get_altitude(vec2 const &new_position_in_plane)
{
    float i_float = parameters.rows * (new_position_in_plane.x - x_min) / (x_max - x_min);
    float j_float = parameters.columns * (new_position_in_plane.y - y_min) / (y_max - y_min);
    double delta_z;
    if (user_reference->sneak) delta_z = player_height / 2;
    else delta_z = player_height;
    int i = int(i_float);
    int j = int(j_float);
    float z_1, z_2, z_3, z_4, z_5, dx, dy;
    dx = i_float - int(i_float);
    dy = j_float - int(j_float);
    if (dx > dy)
    {
        z_1 = terrain_mesh.position[i * parameters.columns + j].z;
        z_2 = terrain_mesh.position[(i + 1) * parameters.columns + j].z;
        z_3 = terrain_mesh.position[(i + 1) * parameters.columns + j + 1].z;
        z_4 = z_1 + dx * (z_2 - z_1);
        z_5 = z_1 + (z_3 - z_1) * dx;
        return z_4 + (z_5 - z_4) * dy / dx + delta_z;
    }
    else
    {
        z_1 = terrain_mesh.position[i * parameters.columns + j].z;
        z_2 = terrain_mesh.position[i * parameters.columns + j + 1].z;
        z_3 = terrain_mesh.position[(i + 1) * parameters.columns + j + 1].z;
        z_4 = z_2 + dx * (z_3 - z_2);
        z_5 = z_1 + (z_3 - z_1) * dx;
        z_4 + (z_5 - z_4) * (1 - dy) / (1 - dx) + delta_z;
        return z_4 + (z_5 - z_4) * (1 - dy) / (1 - dx) + delta_z;
    }
}

float desert::profile(vec2 const& position_in_plane) 
{
    float transition_down = 0.8;
    float transition_up = 0.9;
    float z_min = -0.2;
    float z_max = 0.2;
    float coord_x = std::max((position_in_plane.x - x_min) / (x_max - x_min), (x_max - position_in_plane.x) / (x_max - x_min));
    float coord_y = std::max((position_in_plane.y - y_min) / (y_max - y_min), (y_max - position_in_plane.y) / (y_max - y_min));
    float coord = std::max(coord_x, coord_y);
    if (coord < transition_down) return z_max;
    else if (coord < transition_up) {
        float t = 0.5 * (1 + cos(PI / (transition_up - transition_down) * (coord - transition_down)));
        return t * z_max + (1 - t) * z_min;
    }
    else return z_min;
}