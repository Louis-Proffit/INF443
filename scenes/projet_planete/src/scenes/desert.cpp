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

    // Configuration de la caméra
    camera_m.position_camera = vec3(0, 0, 0);
    camera_m.manipulator_set_altitude(get_altitude(camera_m.position_camera.xy()));
    camera_c.distance_to_center = 2.5f;
    camera_c.look_at({ 4,3,2 }, { 0,0,0 }, { 0,0,1 });
    m_activated = true;

    // Configuration de la lumière
    light = sun_visual.transform.translate;
}

desert::~desert() {}

void desert::display_visual()
{
    user_reference->timer.update();
    float const time = user_reference->timer.t;
    light = sun_visual.transform.translate;

    GLuint normal_shader = open_shader("normal");
    GLuint sun_shader = open_shader("sun");

    glUseProgram(normal_shader);
    opengl_uniform(normal_shader, "projection", projection);
    if (m_activated) opengl_uniform(normal_shader, "view", camera_m.matrix_view());
    else             opengl_uniform(normal_shader, "view", camera_c.matrix_view());
    opengl_uniform(normal_shader, "light", light);

    glUseProgram(sun_shader);
    opengl_uniform(sun_shader, "projection", projection);
    if (m_activated) opengl_uniform(sun_shader, "view", camera_m.matrix_view());
    else             opengl_uniform(sun_shader, "view", camera_c.matrix_view());;
    opengl_uniform(sun_shader, "light", light);

    draw(visual, this);
    if (user_reference->draw_wireframe) draw_wireframe(visual, this);
    /*draw(sun_visual, this);
    if (user_reference->draw_wireframe) draw_wireframe(sun_visual, this);*/
    skybox.display_skybox(this);

    
}

void desert::update_visual()
{
    vec2 const& p0 = user_reference->mouse_prev;
    vec2 const& p1 = user_reference->mouse_curr;
    if (m_activated) {
        vec2 dp(0, 0);

        if (!user_reference->cursor_on_gui) {
            if (user_reference->state.mouse_click_left && !user_reference->state.key_ctrl) {
                camera_m.manipulator_rotate_2_axis(p1.y - p0.y, p1.x - p0.x);
            }
        }

        if (user_reference->state.key_up) dp.y += 1;
        if (user_reference->state.key_down) dp.y -= 1;
        if (user_reference->state.key_left) dp.x -= 1;
        if (user_reference->state.key_right) dp.x += 1;

        int fps = user_reference->fps_record.fps;
        if (fps <= 0) dp *= 0;
        else dp = dp *= user_reference->player_speed / fps;

        camera_m.manipulator_set_translation(dp);
        float new_z = get_altitude(camera_m.position_camera.xy());
        camera_m.manipulator_set_altitude(new_z);
    }
    else {
        if (!user_reference->cursor_on_gui)
        {
            if (user_reference->state.mouse_click_left && !user_reference->state.key_ctrl)
                camera_c.manipulator_rotate_trackball(p0, p1);
            if (user_reference->state.mouse_click_left && user_reference->state.key_ctrl)
                camera_c.manipulator_translate_in_plane(p1 - p0);
            if (user_reference->state.mouse_click_right)
                camera_c.manipulator_scale_distance_to_center((p1 - p0).y);
        }
    }
    
    user_reference->mouse_prev = p1;

    if (height_updated)
    {
        for (int i = 0; i < parameters.rows; i++) {
            for (int j = 0; j < parameters.columns; j++) mesh.position[parameters.columns * i + j].z = height_data[i][j] * horizontal_scale;
        }
        visual.update_position(mesh.position);
        height_updated = false;
    }
}

void desert::display_interface()
{
    if (ImGui::Button("Retour maison")) {
        swap_function(scene_type::PLANET);
        std::cout << "swapped" << std::endl;
        return;
    }
    if (m_activated) m_activated = !ImGui::Button("Camera aerienne");
    else m_activated = ImGui::Button("Camera fpv");

    ImGui::Checkbox("Frame", &user_reference->display_frame);
    ImGui::Checkbox("Wireframe", &user_reference->draw_wireframe);
    height_updated = ImGui::SliderFloat("Echelle horizontale", &horizontal_scale, 0, 2.0f, "%.3f", 2);
    ImGui::SliderFloat("Vitesse de déplacement", &user_reference->player_speed, 0.1, 2.0f, "%.3f", 2);
}

void desert::set_terrain()
{
    parameters = heightmap_parameters{ 0, 0, -5, -5, 5, 5 };
    horizontal_scale = 0.2f;
    height_data = generateFileHeightData("assets/heightmaps/desert.png", horizontal_scale);
    mesh = createFromHeightData(height_data, parameters);
    visual = mesh_drawable(mesh, open_shader("normal"));

    image_raw texture = image_load_png("assets/textures/sand_texture.png");
    GLuint texture_id = opengl_texture_to_gpu(texture, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
    visual.texture = texture_id;
}

void desert::set_skybox() {
    skybox.init_skybox(vec3(0, 0, 0), 10, "desert", open_shader("normal"));
}

void desert::set_sun()
{
    sun_mesh = mesh_primitive_sphere(sun_radius);
    sun_mesh.flip_connectivity();
    sun_visual = mesh_drawable(sun_mesh, open_shader("sun"));
    /*sun_visual.shading.phong.ambient = 100.0f;
    sun_visual.shading.phong.diffuse = 100.0f;
    sun_visual.shading.phong.specular = 100.0f;
    sun_visual.shading.phong.specular_exponent = 0.01f;*/
    sun_visual.shading.color = vec3(1.0, 1.0, 0.0);
}

float desert::get_altitude(vec2 const& new_position_in_plane) 
{
    float i_float = parameters.rows * (new_position_in_plane.x - parameters.min_x) / (parameters.max_x - parameters.min_x);
    float j_float = parameters.columns * (new_position_in_plane.y - parameters.min_y) / (parameters.max_y - parameters.min_y);
    int i = int(i_float);
    int j = int(j_float);
    float z_1, z_2, z_3, z_4, z_5, dx, dy;
    dx = i_float - int(i_float);
    dy = j_float - int(j_float);
    if (dx > dy) {
        z_1 = mesh.position[i * parameters.columns + j].z;
        z_2 = mesh.position[(i + 1) * parameters.columns + j].z;
        z_3 = mesh.position[(i + 1) * parameters.columns + j + 1].z;
        z_4 = z_1 + dx * (z_2 - z_1);
        z_5 = z_1 + (z_3 - z_1) * dx;
        if (user_reference -> sneak) return player_height / 2 + z_4 + (z_5 - z_4) * dy / dx;
        return player_height + z_4 + (z_5 - z_4) * dy / dx;
    }
    else {
        z_1 = mesh.position[i * parameters.columns + j].z;
        z_2 = mesh.position[i * parameters.columns + j + 1].z;
        z_3 = mesh.position[(i + 1) * parameters.columns + j + 1].z;
        z_4 = z_2 + dx * (z_3 - z_2);
        z_5 = z_1 + (z_3 - z_1) * dx;
        if (user_reference->sneak) return player_height / 2 + z_4 + (z_5 - z_4) * (1 - dy) / (1 - dx);
        return player_height + z_4 + (z_5 - z_4) * (1 - dy) / (1 - dx);
    }
}