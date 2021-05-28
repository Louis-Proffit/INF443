#include "field.hpp"

#include <list>
#include <set>

using namespace vcl;

countryside::countryside(user_parameters *user, std::function<void(scene_type)> _swap_function) : environement(user, _swap_function)
{
    // Configuration des visuels
    set_terrain();
    set_skybox();
    set_sun();

    // Configuration de la cam�ra
    camera_m.position_camera = vec3(0, 0, 0);
    camera_m.manipulator_set_altitude(get_altitude(camera_m.position_camera.xy()));
    camera_c.distance_to_center = 2.5f;
    camera_c.look_at({4, 3, 2}, {0, 0, 0}, {0, 0, 1});
    m_activated = true;

    // Configuration de la lumi�re
    light = vec3(1.0f, 1.0f, 1.0f);
}

void countryside::display_visual()
{
    glClearColor(0.256f, 0.256f, 0.256f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    user_reference->timer.update();
    float const time = user_reference->timer.t;
    if (m_activated)
        light = camera_m.position();
    else
        light = camera_c.position();

    GLuint normal_shader = get_shader(shader_type::NORMAL);
    GLuint sun_shader = get_shader(shader_type::SUN);

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

    for (mesh_drawable field_visual : fields_visuals)
        draw(field_visual, this);
    for (mesh_drawable path_visual : paths_visuals)
        draw(path_visual, this);
    if (user_reference->draw_wireframe)
        for (mesh_drawable field_visual : fields_visuals)
            draw_wireframe(field_visual, this);
    if (user_reference->draw_wireframe)
        for (mesh_drawable path_visual : paths_visuals)
            draw_wireframe(path_visual, this);

    skybox.display_skybox(this);
    draw(sun_visual, this);
}

void countryside::update_visual()
{
    super::update_visual();
}

void countryside::display_interface()
{
    super::display_interface();
}

void countryside::set_terrain()
{
    // Create fields and paths
    fields.push_back({mesh_primitive_quadrangle({x_min, y_min, 0}, {x_max, y_min, 0}, {x_max, y_max, 0}, {x_min, y_max, 0})});
    bool subdivide_again = true;
    int current_subdivisions = 0;
    while (subdivide_again)
    {
        subdivide_again = subdivide(current_subdivisions);
        current_subdivisions++;
    }
    set_types();

    // Subdivide paths and fields
    for (int i = 0; i < paths.size(); i++)
    {
        mesh quadrangle = paths[i];
        paths[i] = subdivide_path(quadrangle);
        //quadrangle.~mesh();
        //delete quadrangle;
    }
    for (int i = 0; i < fields.size(); i++)
    {
        mesh quadrangle = fields[i].field_mesh;
        fields[i].field_mesh = subdivide_field(quadrangle);
        //quadrangle.~mesh();
    }

    // Shuffle meshes
    shuffle();

    GLuint normal_shader = scene_visual::get_shader(shader_type::NORMAL);
    fields_visuals.resize(fields.size());
    paths_visuals.resize(paths.size());
    for (int i = 0; i < fields.size(); i++)
        fields_visuals[i] = mesh_drawable(fields[i].field_mesh, normal_shader);
    for (int i = 0; i < paths.size(); i++)
        paths_visuals[i] = mesh_drawable(paths[i], normal_shader);
    set_textures();
}

bool countryside::subdivide(int current_subdivisions)
{
    bool answer = false;
    std::vector<field> new_fields;
    float rand_1, rand_2;
    vec3 middle_1, middle_2, middle_1_minus, middle_1_plus, middle_2_minus, middle_2_plus;
    for (field _field : fields)
    {
        rand_1 = (1 - random_compression) / 2 + random_compression * float(rand()) / RAND_MAX;
        rand_2 = (1 - random_compression) / 2 + random_compression * float(rand()) / RAND_MAX;
        if (current_subdivisions % 2 == 0)
        {
            middle_1 = rand_1 * _field.field_mesh.position[0] + (1 - rand_1) * _field.field_mesh.position[1];
            middle_2 = rand_2 * _field.field_mesh.position[2] + (1 - rand_2) * _field.field_mesh.position[3];
            if (norm(middle_1 - _field.field_mesh.position[0]) > field_min_dimension && norm(middle_1 - _field.field_mesh.position[1]) > field_min_dimension && norm(middle_2 - _field.field_mesh.position[2]) > field_min_dimension && norm(middle_2 - _field.field_mesh.position[3]) > field_min_dimension)
            {
                answer = true;
                middle_1_minus = path_proportion * _field.field_mesh.position[0] + (1 - path_proportion) * middle_1;
                middle_1_plus = path_proportion * _field.field_mesh.position[1] + (1 - path_proportion) * middle_1;
                middle_2_minus = path_proportion * _field.field_mesh.position[2] + (1 - path_proportion) * middle_2;
                middle_2_plus = path_proportion * _field.field_mesh.position[3] + (1 - path_proportion) * middle_2;
                new_fields.push_back({mesh_primitive_quadrangle(_field.field_mesh.position[0], middle_1_minus, middle_2_plus, _field.field_mesh.position[3])});
                new_fields.push_back({mesh_primitive_quadrangle(_field.field_mesh.position[2], middle_2_minus, middle_1_plus, _field.field_mesh.position[1])});
                paths.push_back(mesh_primitive_quadrangle(middle_1_minus, middle_1_plus, middle_2_minus, middle_2_plus));
            }
            else
                new_fields.push_back(_field);
        }
        else
        {
            middle_1 = rand_1 * _field.field_mesh.position[1] + (1 - rand_1) * _field.field_mesh.position[2];
            middle_2 = rand_2 * _field.field_mesh.position[3] + (1 - rand_2) * _field.field_mesh.position[0];
            if (norm(middle_1 - _field.field_mesh.position[1]) > field_min_dimension && norm(middle_1 - _field.field_mesh.position[2]) > field_min_dimension && norm(middle_2 - _field.field_mesh.position[3]) > field_min_dimension && norm(middle_2 - _field.field_mesh.position[0]) > field_min_dimension)
            {
                answer = true;
                middle_1_minus = path_proportion * _field.field_mesh.position[1] + (1 - path_proportion) * middle_1;
                middle_1_plus = path_proportion * _field.field_mesh.position[2] + (1 - path_proportion) * middle_1;
                middle_2_minus = path_proportion * _field.field_mesh.position[3] + (1 - path_proportion) * middle_2;
                middle_2_plus = path_proportion * _field.field_mesh.position[0] + (1 - path_proportion) * middle_2;
                new_fields.push_back({mesh_primitive_quadrangle(_field.field_mesh.position[0], _field.field_mesh.position[1], middle_1_minus, middle_2_plus)});
                new_fields.push_back({mesh_primitive_quadrangle(middle_1_plus, _field.field_mesh.position[2], _field.field_mesh.position[3], middle_2_minus)});
                paths.push_back(mesh_primitive_quadrangle(middle_1_minus, middle_1_plus, middle_2_minus, middle_2_plus));
            }
            else
                new_fields.push_back(_field);
        }
    }
    fields.clear();
    fields = new_fields;
    current_subdivisions++;
    return answer;
}

void countryside::set_types()
{
    for (int i = 0; i < fields.size(); i++)
    {
        fields[i].type = static_cast<field_type>(int(rand_interval() * 12));
    }
}

void countryside::set_textures()
{
    for (int i = 0; i < fields.size(); i++)
        switch (fields[i].type) {
        case field_type::UN:
            fields_visuals[i].texture = scene_visual::get_texture(texture_type::FIELD_2);
            break;
        case field_type::DEUX:
            fields_visuals[i].texture = scene_visual::get_texture(texture_type::FIELD_2);
            break;
        case field_type::TROIS:
            fields_visuals[i].texture = scene_visual::get_texture(texture_type::FIELD_3);
            break;
        case field_type::QUATRE:
            fields_visuals[i].texture = scene_visual::get_texture(texture_type::FIELD_4);
            break;
        }
    for (int i = 0; i < fields.size(); i++)
        paths_visuals[i].texture = scene_visual::get_texture(texture_type::FIELD_1);
}

void countryside::set_skybox()
{
    skybox.init_skybox(vec3(0, 0, 0), 10, skybox_type::FLEUVE, get_shader(shader_type::NORMAL));
}

void countryside::set_sun()
{
    sun_visual = mesh_drawable(mesh_primitive_sphere(sun_radius), get_shader(shader_type::SUN));
    sun_visual.shading.color = vec3(1.0, 1.0, 0.0);
}

float countryside::get_altitude(vcl::vec2 const &position_in_plane)
{
    if (!user_reference->sneak)
        return path_z_max + player_height + parameters.height * noise_perlin(position_in_plane, parameters.octaves, parameters.persistency, parameters.frequency_gain);
    else
        return path_z_max + player_height / 2 + parameters.height * noise_perlin(position_in_plane, parameters.octaves, parameters.persistency, parameters.frequency_gain);
}

mesh countryside::subdivide_path(vcl::mesh quadrangle)
{
    vec3 up_shift = vec3(0, 0, path_z_max);
    vec3 down_shift = vec3(0, 0, path_z_min);
    int Nx = std::max(2, int(norm(quadrangle.position[0] - quadrangle.position[1]) * field_subdivisions / (field_min_dimension * (x_max - x_min))));
    int Ny = std::max(2, int(norm(quadrangle.position[1] - quadrangle.position[2]) * field_subdivisions / (field_min_dimension * (x_max - x_min))));
    int Nz = 3;
    mesh result = mesh_primitive_cubic_grid(
        quadrangle.position[0] + up_shift,
        quadrangle.position[1] + up_shift,
        quadrangle.position[2] + up_shift,
        quadrangle.position[3] + up_shift,
        quadrangle.position[0] + down_shift,
        quadrangle.position[1] + down_shift,
        quadrangle.position[2] + down_shift,
        quadrangle.position[3] + down_shift,
        Nx, Ny, Nz);
    for (int i = 0; i < result.uv.size(); i++)
    { // Compresses the ground texture
        if (Nx > Ny)
            result.uv[i].y *= Nx;
        else
            result.uv[i].x *= Ny;
    }
    return result;
}

mesh countryside::subdivide_field(vcl::mesh quadrangle)
{
    return mesh_primitive_grid(quadrangle.position[0], quadrangle.position[1], quadrangle.position[2], quadrangle.position[3], field_subdivisions, field_subdivisions);
}

void countryside::shuffle()
{
    // Shuffle paths
    for (int i = 0; i < paths.size(); i++) {
        for (int j = 0; j < paths[i].position.size(); j++) {
            paths[i].position[j].z += parameters.height * noise_perlin(paths[i].position[j].xy(), parameters.octaves, parameters.persistency, parameters.frequency_gain);
        }
    }

    for (int i = 0; i < fields.size(); i++) {
        for (int j = 0; j < fields[i].field_mesh.position.size(); j++) {
            fields[i].field_mesh.position[j].z += parameters.height * noise_perlin(fields[i].field_mesh.position[j].xy(), parameters.octaves, parameters.persistency, parameters.frequency_gain);
        }
    }
}