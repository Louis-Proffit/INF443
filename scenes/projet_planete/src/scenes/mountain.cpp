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
    set_water();
    set_snow();

    // Configuration de la cam�ra
    camera_m.position_camera = vec3(0, 0, 0);
    camera_m.manipulator_set_altitude(get_altitude(camera_m.position_camera.xy()));
    camera_c.distance_to_center = 2.5f;
    camera_c.look_at({4, 3, 2}, {0, 0, 0}, {0, 0, 1});
    // Configuration de la lumi�re
    light = sun_visual.transform.translate;
}

void mountain::display_visual()
{
    user_reference->timer.update();
    float const time = user_reference->timer.t;
    display_reflec_refrac(clipPlane);
    glClearColor(0.215f, 0.215f, 0.215f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    display_scene(clipPlane);
    if (m_activated)
        wat.set_Uniforms(fbos.getReflectionTexture(), fbos.getRefractionTexture(), camera_m.position(), fbos.movefactor);
    else
        wat.set_Uniforms(fbos.getReflectionTexture(), fbos.getRefractionTexture(), camera_c.position(), fbos.movefactor);

    GLuint partic_mountain_shader = get_shader(shader_type::PARTICLE_MOUNTAIN);

    glUseProgram(partic_mountain_shader);
    opengl_uniform(partic_mountain_shader, "projection", projection);
    if (m_activated)
        opengl_uniform(partic_mountain_shader, "view", camera_m.matrix_view());
    else
        opengl_uniform(partic_mountain_shader, "view", camera_c.matrix_view());

    if (m_activated)
        snow.updateParticles(camera_m.position());
    else
        snow.updateParticles(camera_c.position());
    snow.updateShadVbos(this);
    glUseProgram(water_shader);
    opengl_uniform(water_shader, "projection", projection);
    if (m_activated)
        opengl_uniform(water_shader, "view", camera_m.matrix_view());
    else
        opengl_uniform(water_shader, "view", camera_c.matrix_view());
    opengl_uniform(water_shader, "light", light);
    opengl_uniform(water_shader, "fog_falloff", 0.06f);
    draw(wat.waterd, this);
}

void mountain::update_visual()
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
        terrain_visual.update_normal(terrain_mesh.normal);
        height_updated = false;
    }
}

void mountain::display_interface()
{
    height_updated = ImGui::SliderFloat("Echelle horizontale", &horizontal_scale, 0, 3.0f, "%.3f", 2);
    super::display_interface();
}

void mountain::set_terrain()
{
    parameters = heightmap_parameters{0, 0, x_min, y_min, x_max, y_max};
    horizontal_scale = 2.0f;
    height_data = generateFileHeightData("../assets/heightmaps/mountain3.png", horizontal_scale);
    terrain_mesh = createFromHeightData(height_data, parameters);
    for (int i = 0; i < terrain_mesh.position.size(); i++)
        terrain_mesh.position[i].z += profile(terrain_mesh.position[i].xy());
    terrain_visual = mesh_drawable(terrain_mesh, get_shader(shader_type::MOUNTAIN));

    GLuint texture_id = scene_visual::get_texture(texture_type::ROCK);
    terrain_visual.texture = texture_id;
}

void mountain::set_skybox()
{
    skybox.init_skybox(vec3(0, 0, 0), x_max - x_min + y_max - y_min, skybox_type::NEIGE, get_shader(shader_type::MOUNTAIN));
}

void mountain::set_sun()
{
    sun_mesh = mesh_primitive_sphere(sun_radius);
    sun_mesh.flip_connectivity();
    sun_visual = mesh_drawable(sun_mesh, get_shader(shader_type::SUN));
    sun_visual.shading.color = vec3(1.0, 1.0, 0.0);
}

float mountain::get_altitude(vec2 const &new_position_in_plane)
{
    float i_float = parameters.rows * (new_position_in_plane.x - x_min) / (x_max - x_min);
    float j_float = parameters.columns * (new_position_in_plane.y - y_min) / (y_max - y_min);
    double delta_z;
    if (user_reference->sneak)
        delta_z = player_height / 2;
    else
        delta_z = player_height;
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

float mountain::profile(vec2 const &position_in_plane)
{
    float transition_down = 0.8;
    float transition_up = 0.9;
    float z_min = -0.2;
    float z_max = 0.2;
    float coord_x = std::max((position_in_plane.x - x_min) / (x_max - x_min), (x_max - position_in_plane.x) / (x_max - x_min));
    float coord_y = std::max((position_in_plane.y - y_min) / (y_max - y_min), (y_max - position_in_plane.y) / (y_max - y_min));
    float coord = std::max(coord_x, coord_y);
    if (coord < transition_down)
        return z_max;
    else if (coord < transition_up)
    {
        float t = 0.5 * (1 + cos(PI / (transition_up - transition_down) * (coord - transition_down)));
        return t * z_max + (1 - t) * z_min;
    }
    else
        return z_min;
}

void mountain::display_scene(vec4 clipPlane)
{
    glClearColor(0.256f, 0.256f, 0.256f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    light = sun_visual.transform.translate;

    GLuint mountain_shader = get_shader(shader_type::MOUNTAIN);
    GLuint sun_shader = get_shader(shader_type::SUN);

    GLuint texture_snow = scene_visual::get_texture(texture_type::SNOW);

    glUseProgram(mountain_shader);
    opengl_uniform(mountain_shader, "projection", projection);
    if (m_activated)
        opengl_uniform(mountain_shader, "view", camera_m.matrix_view());
    else
        opengl_uniform(mountain_shader, "view", camera_c.matrix_view());
    opengl_uniform(mountain_shader, "light", light);
    opengl_uniform(mountain_shader, "plane", clipPlane);
    opengl_uniform(mountain_shader, "fog_falloff", 0.1f);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_snow);
    opengl_uniform(mountain_shader, "image_texture_snow", 1);

    glUseProgram(sun_shader);
    opengl_uniform(sun_shader, "projection", projection);
    if (m_activated)
        opengl_uniform(sun_shader, "view", camera_m.matrix_view());
    else
        opengl_uniform(sun_shader, "view", camera_c.matrix_view());
    ;
    opengl_uniform(sun_shader, "light", light);
    opengl_uniform(sun_shader, "plane", clipPlane);

    draw(terrain_visual, this);
    if (user_reference->draw_wireframe)
        draw_wireframe(terrain_visual, this);

    /*draw(sun_visual, this);
    if (user_reference->draw_wireframe) draw_wireframe(sun_visual, this);*/
    glUseProgram(mountain_shader);
    opengl_uniform(mountain_shader, "fog_falloff", 0.001f);

    skybox.display_skybox(this);
}

void mountain::display_reflec_refrac(vec4 clipPlane)
{
    // Water Refraction rendering
    fbos.movefactor += (0.3 / 58.0);
    glEnable(GL_CLIP_DISTANCE0);
    fbos.bindRefractionFrameBuffer();

    glClearColor(0.215f, 0.215f, 0.215f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    display_scene(-clipPlane);

    // Water Reflection rendering

    fbos.bindReflectionFrameBuffer();
    if (m_activated)
    {
        float pos = 2 * (camera_m.position().z - wat.waterHeight);
        vec3 eye = camera_m.position();
        camera_m.manipulator_rotate_2_axis(-2 * camera_m.rotation_orthogonal, 0);
        camera_m.position_camera = eye - vec3(0, 0, pos);

        glClearColor(0.215f, 0.215f, 0.215f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        display_scene(clipPlane);

        camera_m.manipulator_rotate_2_axis(-2 * camera_m.rotation_orthogonal, 0);
        camera_m.position_camera = eye;
    }
    else
    {
        vec3 eye = camera_c.position();
        float pos = 2 * (eye.z - wat.waterHeight);
        // std::cout << camera_c.center_of_rotation;
        camera_c.look_at(eye - vec3(0, 0, pos), camera_c.center_of_rotation, vec3(0, 0, 1));

        glClearColor(0.215f, 0.215f, 0.215f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        display_scene(clipPlane);
        camera_c.look_at(eye, camera_c.center_of_rotation, vec3(0, 0, 1));
    }
    fbos.unbindCurrentFrameBuffer();
    glDisable(GL_CLIP_DISTANCE0);
}

void mountain::set_water()
{
    wat.init_water(scene_visual::water_shader);
    fbos.initWaterFrameBuffers();
    clipPlane = vec4(0, 0, 1, -wat.waterHeight);
}

void mountain::set_snow()
{
    snow = Particles(nb_particles, "snowflakes", x_min, x_max, y_min, y_max);
    snow.initVaoVbo(scene_visual::get_shader(shader_type::PARTICLE_MOUNTAIN));
}