#include "field.hpp"

#include <list>
#include <set>

using namespace vcl;

countryside::countryside(user_parameters *user, std::function<void(scene_type)> _swap_function) : environement(user, _swap_function)
{
    x_min = -2.0;
    x_min = -2.0;
    y_max = 2.0;
    y_max = 2.0;

    // Configuration des visuels
    set_terrain();
    set_sand();
    set_tractor();
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

    draw(path_visual, this);
    for (mesh_drawable field_visual : fields_visuals)
        draw(field_visual, this);
    for (vec3 position : tractor_positions)
    {
        tractor_visual.transform.translate = position;
        draw(tractor_visual, this);
    }
    draw(sand_visual, this);
    if (user_reference->draw_wireframe)
    {
        for (mesh_drawable field_visual : fields_visuals)
            draw_wireframe(field_visual, this);
        draw_wireframe(path_visual, this);
        draw_wireframe(sand_visual, this);
        draw_wireframe(sun_visual, this);
    }

    skybox.display_skybox(this);
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

    set_border();
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

    // Merge paths
    mesh path;
    for (int i = 0; i < paths.size(); i++)
    {
        path.push_back(paths[i]);
    }

    GLuint normal_shader = scene_visual::get_shader(shader_type::NORMAL);
    fields_visuals.resize(fields.size());
    for (int i = 0; i < fields.size(); i++)
        fields_visuals[i] = mesh_drawable(fields[i].field_mesh, normal_shader);
    path_visual = mesh_drawable(path, normal_shader);
    set_textures();
}

void countryside::set_sand()
{
    mesh sand;
    int N1 = 100;
    int N2 = 500;
    float delta = (x_max - x_min) * sand_proportion / 2.0;
    mesh side = mesh_primitive_grid(vec3(x_min - delta, y_min, 0), vec3(x_max + delta, y_min, 0), vec3(x_max + delta, y_min - delta, 0), vec3(x_min - delta, y_min - delta, 0), N2, N1); // Bas
    sand.push_back(side);
    //side.~mesh();
    side = mesh_primitive_grid(vec3(x_min, y_max + delta, 0), vec3(x_min, y_min - delta, 0), vec3(x_min - delta, y_min - delta, 0), vec3(x_min - delta, y_max + delta, 0), N2, N1); // Gauche
    sand.push_back(side);
    //side.~mesh();
    side = mesh_primitive_grid(vec3(x_max, y_max + delta, 0), vec3(x_max, y_min - delta, 0), vec3(x_max + delta, y_min - delta, 0), vec3(x_max + delta, y_max + delta, 0), N2, N1); // Gauche
    sand.push_back(side);
    //side.~mesh();
    side = mesh_primitive_grid(vec3(x_min - delta, y_max, 0), vec3(x_max + delta, y_max, 0), vec3(x_max + delta, y_max + delta, 0), vec3(x_min - delta, y_max + delta, 0), N2, N1); // Haut
    sand.push_back(side);

    for (int i = 0; i < sand.position.size(); i++)
    {
        sand.position[i].z += profile(sand.position[i].xy()) + parameters.height * noise_perlin(sand.position[i].xy(), parameters.octaves, parameters.persistency, parameters.frequency_gain);
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

void countryside::set_border()
{
    float delta = (x_max - x_min) * border_proportion / 2.0;
    mesh side = mesh_primitive_quadrangle(vec3(x_min - delta, y_min, 0), vec3(x_max + delta, y_min, 0), vec3(x_max + delta, y_min - delta, 0), vec3(x_min - delta, y_min - delta, 0)); // Bas
    paths.push_back(side);
    // //side.~mesh();
    side = mesh_primitive_quadrangle(vec3(x_min, y_max + delta, 0), vec3(x_min, y_min - delta, 0), vec3(x_min - delta, y_min - delta, 0), vec3(x_min - delta, y_max + delta, 0)); // Gauche
    paths.push_back(side);
    //side.~mesh();
    side = mesh_primitive_quadrangle(vec3(x_max, y_max + delta, 0), vec3(x_max, y_min - delta, 0), vec3(x_max + delta, y_min - delta, 0), vec3(x_max + delta, y_max + delta, 0)); // Gauche
    paths.push_back(side);
    // side.~mesh();
    side = mesh_primitive_quadrangle(vec3(x_min - delta, y_max, 0), vec3(x_max + delta, y_max, 0), vec3(x_max + delta, y_max + delta, 0), vec3(x_min - delta, y_max + delta, 0)); // Haut
    paths.push_back(side);
}

void countryside::set_tractor()
{
    mesh tractor = mesh_load_file_obj("../assets/objects/tractor/tractor.obj");
    GLuint normal_shader = scene_visual::get_shader(shader_type::NORMAL);
    tractor_visual = mesh_drawable(tractor, normal_shader);
    tractor_visual.transform.scale = 0.02;
    tractor_visual.texture = get_texture(texture_type::LOWPOLY);

    /* Set transform */
    tractor_positions.resize(number_of_tractors);
    vec3 position;
    for (int i = 0; i < number_of_tractors; i++)
    {
        position = vec3(0.9 * x_min + 0.9 * rand_interval() * (x_max - x_min), 0.9 * y_min + 0.9 * rand_interval() * (y_max - y_min), 0);
        position.z = parameters.height * noise_perlin(position.xy(), parameters.octaves, parameters.persistency, parameters.frequency_gain) + profile(position.xy());
        tractor_positions[i] = position;
        position.~vec3();
    }
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
        fields[i].type = static_cast<field_type>(int(rand_interval() * 4));
    }
}

void countryside::set_textures()
{
    for (int i = 0; i < fields.size(); i++)
    {
        switch (fields[i].type)
        {
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
    }
    path_visual.texture = scene_visual::get_texture(texture_type::FIELD_1);
}

void countryside::set_skybox()
{
    skybox.init_skybox(vec3(0, 0, 0), x_max - x_min, skybox_type::FLEUVE, get_shader(shader_type::NORMAL));
}

void countryside::set_sun()
{
    sun_visual = mesh_drawable(mesh_primitive_sphere(sun_radius), get_shader(shader_type::SUN));
    sun_visual.shading.color = vec3(1.0, 1.0, 0.0);
}

float countryside::get_altitude(vcl::vec2 const &position_in_plane)
{
    float dz = profile(position_in_plane);
    if (!user_reference->sneak)
        return dz + player_height + parameters.height * noise_perlin(position_in_plane, parameters.octaves, parameters.persistency, parameters.frequency_gain);
    else
        return dz + player_height / 2 + parameters.height * noise_perlin(position_in_plane, parameters.octaves, parameters.persistency, parameters.frequency_gain);
}

mesh countryside::subdivide_path(vcl::mesh quadrangle)
{
    int Nx = std::max(10, int(50 * norm(quadrangle.position[0] - quadrangle.position[1])));
    int Ny = std::max(10, int(50 * norm(quadrangle.position[1] - quadrangle.position[2]))); // Plus grand
    vec3 delta = vec3(0, 0, path_dz);
    mesh result = mesh_primitive_cubic_grid(
        quadrangle.position[0] + delta,
        quadrangle.position[1] + delta,
        quadrangle.position[2] + delta,
        quadrangle.position[3] + delta,
        quadrangle.position[0] - delta,
        quadrangle.position[1] - delta,
        quadrangle.position[2] - delta,
        quadrangle.position[3] - delta,
        Ny, 3, Nx);

    for (int i = 0; i < result.uv.size(); i++)
    {
        result.uv[i].x = 2 * result.position[i].x + 2 * result.position[i].z;
        result.uv[i].y = 2 * result.position[i].y + 2 * result.position[i].z;
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
    for (int i = 0; i < paths.size(); i++)
    {
        for (int j = 0; j < paths[i].position.size(); j++)
        {
            paths[i].position[j].z += parameters.height * noise_perlin(paths[i].position[j].xy(), parameters.octaves, parameters.persistency, parameters.frequency_gain);
            paths[i].position[j].z += profile(paths[i].position[j].xy());
        }
    }

    // Shuffle fields
    for (int i = 0; i < fields.size(); i++)
    {
        for (int j = 0; j < fields[i].field_mesh.position.size(); j++)
        {
            fields[i].field_mesh.position[j].z += parameters.height * noise_perlin(fields[i].field_mesh.position[j].xy(), parameters.octaves, parameters.persistency, parameters.frequency_gain);
            fields[i].field_mesh.position[j].z += profile(fields[i].field_mesh.position[j].xy());
        }
    }
}

float countryside::profile(vec2 const &position_in_plane)
{
    float transition_down = 1.0;
    float transition_up = 1.2;
    float z_min = -0.4;
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