#include "planet.hpp"

#include <list>
#include <set>

using namespace vcl;

planet::planet(user_parameters* user, std::function<void(scene_type)> _swap_function) : scene_visual(user, _swap_function)
{
    GLuint planet_shader = open_shader("planet");
    GLuint normal_shader = open_shader("normal");
    GLuint sun_shader = open_shader("sun");

    // Initialisation des shaders
    glUseProgram(planet_shader);
    opengl_uniform(planet_shader, "pulse_vertical", sea_movement_pulse_vertical);
    opengl_uniform(planet_shader, "pulse_horizontal", sea_movement_pulse_horizontal);
    opengl_uniform(planet_shader, "height_vertical", sea_movement_height_vertical);
    opengl_uniform(planet_shader, "height_horizontal", sea_movement_height_horizontal);

    // Configuration des visuels
    set_planet();
    set_islands();
    set_skybox();
    set_sun();

    // Configuration de la caméra
    camera.distance_to_center = 2.5f;
    camera.look_at({ 4,3,2 }, { 0,0,0 }, { 0,0,1 });

    // Configuration de la lumière
    light = vec3(1.0f, 1.0f, 1.0f);

    // Configuration du swap
    swap_function = _swap_function;
}

planet::~planet() {}

void planet::display_visual()
{
    user_reference->timer.update();
    float const time = user_reference->timer.t;
    light = camera.position();

    GLuint planet_shader = open_shader("planet");
    GLuint normal_shader = open_shader("normal");
    GLuint sun_shader = open_shader("sun");

    glUseProgram(planet_shader);
    opengl_uniform(planet_shader, "time", time);
    opengl_uniform(planet_shader, "projection", projection);
    opengl_uniform(planet_shader, "view", camera.matrix_view());
    opengl_uniform(planet_shader, "light", light);

    glUseProgram(normal_shader);
    opengl_uniform(normal_shader, "projection", projection);
    opengl_uniform(normal_shader, "view", camera.matrix_view());
    opengl_uniform(normal_shader, "light", light);

    glUseProgram(sun_shader);
    opengl_uniform(sun_shader, "projection", projection);
    opengl_uniform(sun_shader, "view", camera.matrix_view());
    opengl_uniform(sun_shader, "light", light);


    draw(planet_visual, this);

    if (user_reference->draw_wireframe) draw_wireframe(planet_visual, this);
    for (size_t i = 0; i < number_of_islands; i++) {
        if (picking.active && picking.index == i)
            islands_visuals[i].shading.color = vec3(1, 0, 0);
        else
            islands_visuals[i].shading.color = vec3(1, 1, 1);
        draw(islands_visuals[i], this);
    }

    skybox.display_skybox(this);

    draw(sun_visual, this);
}

void planet::update_visual()
{
    vec2 const& p0 = user_reference->mouse_prev;
    vec2 const& p1 = user_reference->mouse_curr;

    if (!user_reference->cursor_on_gui && !user_reference->state.key_shift) {
        if (user_reference->state.mouse_click_left && !user_reference->state.key_ctrl)
            camera.manipulator_rotate_trackball(p0, p1);
        if (user_reference->state.mouse_click_left && user_reference->state.key_ctrl)
            camera.manipulator_translate_in_plane(p1 - p0);
        if (user_reference->state.mouse_click_right)
            camera.manipulator_scale_distance_to_center((p1 - p0).y);
    }

    user_reference->mouse_prev = p1;

    if (user_reference->state.key_shift)
    {
        if (!user_reference->state.mouse_click_left)
        {

            vec3 const ray_direction = camera_ray_direction(camera.matrix_frame(), projection_inverse, p1);
            vec3 const ray_origin = camera.position();

            intersection_structure intersection = intersection_ray_sphere(ray_origin, ray_direction, { 0, 0, 0 }, sphere_radius);

            if (intersection.valid == true)
            {
                picking.active = false;
                for (int i = 0; i < islands_centers.size(); i++)
                {
                    if (norm(intersection.position - islands_centers[i]) < island_radius)
                    {
                        picking = { true, i };
                        break;
                    }
                }
            }
        }

        if (user_reference->state.mouse_click_left && picking.active) // Action lors du clic
            switch (terrain_types[picking.index]) {
            case scene_type::CITY:
                swap_function(scene_type::CITY);
                break;
            case scene_type::DESERT:
                swap_function(scene_type::DESERT);
                break;
            case scene_type::FIELD:
                swap_function(scene_type::FIELD);
                break;
            case scene_type::FOREST:
                swap_function(scene_type::FOREST);
                break;
            case scene_type::MOUNTAIN:
                swap_function(scene_type::MOUNTAIN);
                break;
            }
    }
    else
        picking.active = false;
}


void planet::display_interface() 
{
    ImGui::Checkbox("Frame", &user_reference->display_frame);
    ImGui::Checkbox("Wireframe", &user_reference->draw_wireframe);
}

void planet::set_planet() 
{
    create_sphere();
    image_raw texture = image_load_png("assets/textures/water_texture.png");
    GLuint texture_id = opengl_texture_to_gpu(texture, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    planet_visual.texture = texture_id;
    planet_visual = mesh_float_drawable(planet_mesh, noise, parrallels, open_shader("planet"));
}

void planet::set_islands() 
{
    islands_centers.resize(number_of_islands);
    terrain_types.resize(number_of_islands);

    vec3 new_center;
    bool correct;
    for (int i = 0; i < number_of_islands; i++){
        /*new_center = get_point_on_sphere(2 * vec3(rand_interval(), rand_interval(), rand_interval()) - vec3(1, 1, 1));*/
        while (true) {
            correct = true;
            new_center = get_point_on_sphere(2 * vec3(rand_interval(), rand_interval(), rand_interval()) - vec3(1, 1, 1));
            for (int j = 0; j < i; j++) {
                if (norm(new_center - islands_centers[j]) < 0.6 * 2 * island_radius) correct = false;
            }
            if (correct) break;
            std::cout << "loop" << std::endl;
        }
        islands_centers[i] = new_center;
        if (i < 5) terrain_types[i] = static_cast<scene_type>(i);
        else terrain_types[i] = static_cast<scene_type>(int(rand_interval() * 5));
    }

    for (int i = 0; i < number_of_islands; i++) {
        for (int j = 0; j < number_of_islands; j++) {
            if (i == j) continue;
            assert_vcl(norm(islands_centers[i] - islands_centers[j]) >= 0.6 * 2 * island_radius, "islands too close");
        }
    }

    image_raw texture = image_load_png("assets/textures/lowpoly_palette.png");
    GLuint texture_id = opengl_texture_to_gpu(texture, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    for (int i = 0; i < number_of_islands; i++) {
        mesh_drawable island_visual;
        mesh island_mesh;

        switch (terrain_types[i]) {
        case scene_type::FOREST :
            island_mesh = mesh_load_file_obj("assets/objects/oak/island_with_oak.obj");
            break;
        case scene_type::CITY:
            island_mesh = mesh_load_file_obj("assets/objects/house/island_with_house.obj");
            break;
        case scene_type::DESERT:
            island_mesh = mesh_load_file_obj("assets/objects/palm/island_with_palm.obj");
            break;
        case scene_type::MOUNTAIN:
            island_mesh = mesh_load_file_obj("assets/objects/mountain/island_with_mountain.obj");
            break;
        case scene_type::FIELD:
            island_mesh = mesh_load_file_obj("assets/objects/tractor/island_with_tractor.obj");
            break;
        }

        curve_mesh(&island_mesh, 0.99 * sphere_radius);
        island_mesh.fill_empty_field();
        island_visual = mesh_drawable(island_mesh, open_shader("normal"));
        island_visual.texture = texture_id;

        island_visual.transform = affine_rts(rotation_around_center(rotation_between_vector(vec3(0, 0, 1), normalize(islands_centers[i])), vec3(0, 0, 0)));
        islands_visuals.push_back(island_visual);
    }
}

void planet::set_skybox() {
    skybox.init_skybox(vec3(0, 0, 0), 10, "space", open_shader("normal"));
}

void planet::set_sun()
{
    sun = mesh_primitive_sphere(sun_radius);
    sun_visual = mesh_drawable(sun, open_shader("sun"));
    sun_visual.shading.color = vec3(1.0, 1.0, 0.0);
}

void planet::create_sphere() 
{

    mesh _mesh = create_isocaedre();

    unsigned int number_of_triangles, number_of_positions;

    int index_1, index_2, index_3;
    int index_middle_1, index_middle_2, index_middle_3;
    vec3 middle_point_1, middle_point_2, middle_point_3;

    std::map<int, int> middle_points; // Associe le hash_code des deux sommets de l'arrête avec l'indice du sommet du milieu

    for (int i = 0; i < sphere_division_steps; i++) {

        middle_points.clear();
        number_of_positions = _mesh.position.size();
        number_of_triangles = _mesh.connectivity.size();

        for (int j = 0; j < number_of_triangles; j++) {

            // Indice des sommets du triangle
            index_1 = _mesh.connectivity[j][0];
            index_2 = _mesh.connectivity[j][1];
            index_3 = _mesh.connectivity[j][2];

            if (middle_points.find(index_1 + number_of_positions * index_2) == middle_points.end()) {
                middle_point_1 = get_point_on_sphere((_mesh.position[index_1] + _mesh.position[index_2]) / 2);
                _mesh.position.push_back(middle_point_1);
                middle_points.insert(std::pair<int, int>(index_1 + number_of_positions * index_2, _mesh.position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_2 + number_of_positions * index_1, _mesh.position.size() - 1));
            }
            if (middle_points.find(index_2 + number_of_positions * index_3) == middle_points.end()) {
                middle_point_2 = get_point_on_sphere((_mesh.position[index_2] + _mesh.position[index_3]) / 2);
                _mesh.position.push_back(middle_point_2);
                middle_points.insert(std::pair<int, int>(index_2 + number_of_positions * index_3, _mesh.position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_3 + number_of_positions * index_2, _mesh.position.size() - 1));
            }
            if (middle_points.find(index_3 + number_of_positions * index_1) == middle_points.end()) {
                middle_point_3 = get_point_on_sphere((_mesh.position[index_3] + _mesh.position[index_1]) / 2);
                _mesh.position.push_back(middle_point_3);
                middle_points.insert(std::pair<int, int>(index_3 + number_of_positions * index_1, _mesh.position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_1 + number_of_positions * index_3, _mesh.position.size() - 1));
            }
        }

        for (int j = 0; j < number_of_triangles; j++) {

            // Indice des sommets du triangle
            index_1 = _mesh.connectivity[j][0];
            index_2 = _mesh.connectivity[j][1];
            index_3 = _mesh.connectivity[j][2];

            // Indice des points milieu du triangle
            index_middle_1 = middle_points.at(index_1 + number_of_positions * index_2);
            index_middle_2 = middle_points.at(index_2 + number_of_positions * index_3);
            index_middle_3 = middle_points.at(index_3 + number_of_positions * index_1);

            _mesh.connectivity[j] = uint3(index_1, index_middle_1, index_middle_3);
            _mesh.connectivity.push_back(uint3(index_middle_1, index_2, index_middle_2));
            _mesh.connectivity.push_back(uint3(index_middle_1, index_middle_2, index_middle_3));
            _mesh.connectivity.push_back(uint3(index_middle_2, index_3, index_middle_3));
        }
    }

    number_of_positions = _mesh.position.size();
    number_of_triangles = _mesh.connectivity.size();

    /* Shuffle mesh */
    float perturbation_amp = 0.3 * norm(_mesh.position[_mesh.connectivity[0].x] - _mesh.position[_mesh.connectivity[0].y]);
    for (int i = 0; i < number_of_positions; i++) _mesh.position[i] = get_point_on_sphere(_mesh.position[i] + perturbation_amp * (2 * vec3(rand_interval(), rand_interval(), rand_interval()) - vec3(1.0, 1.0, 1.0)));

    /* Set final mesh */

    planet_mesh.position.resize(3 * number_of_triangles);
    planet_mesh.color.resize(3 * number_of_triangles);
    planet_mesh.connectivity.resize(number_of_triangles);
    noise.resize(3 * number_of_triangles);
    parrallels.resize(3 * number_of_triangles);

    /* Get triangles maximum distance from the sun */
    float max_distance = DBL_MIN;
    float min_distance = DBL_MAX;
    float distance;

    // Parralels
    buffer<vec3> parrallels_position = buffer<vec3>(number_of_positions);
    for (int i = 0; i < number_of_triangles; i++) {
        index_1 = _mesh.connectivity[i].x;
        index_2 = _mesh.connectivity[i].y;
        index_3 = _mesh.connectivity[i].z;

        parrallels_position[index_1] = _mesh.position[index_2] - _mesh.position[index_1];
        parrallels_position[index_2] = _mesh.position[index_3] - _mesh.position[index_2];
        parrallels_position[index_3] = _mesh.position[index_1] - _mesh.position[index_3];
    }

    // float t;
    for (int i = 0; i < number_of_triangles; i++) {
        index_1 = _mesh.connectivity[i].x;
        index_2 = _mesh.connectivity[i].y;
        index_3 = _mesh.connectivity[i].z;

        planet_mesh.connectivity[i] = uint3(3 * i , 3 * i + 1, 3 * i + 2);

        planet_mesh.position[3 * i + 0] = _mesh.position[index_1];
        planet_mesh.position[3 * i + 1] = _mesh.position[index_2];
        planet_mesh.position[3 * i + 2] = _mesh.position[index_3];

        //t = (distances_to_sun_triangles[i] - min_distance) / (max_distance - min_distance);
        planet_mesh.color[3 * i + 0] = color_sea_high; //t* color_sea_low + (1 - t) * color_sea_high;
        planet_mesh.color[3 * i + 1] = planet_mesh.color[3 * i];
        planet_mesh.color[3 * i + 2] = planet_mesh.color[3 * i];

        noise[3 * i + 0] = noise_perlin(_mesh.position[index_1], 7, 2.0, 2.0);
        noise[3 * i + 1] = noise_perlin(_mesh.position[index_2], 7, 2.0, 2.0);
        noise[3 * i + 2] = noise_perlin(_mesh.position[index_3], 7, 2.0, 2.0);

        parrallels[3 * i + 0] = parrallels_position[index_1];
        parrallels[3 * i + 1] = parrallels_position[index_2];
        parrallels[3 * i + 2] = parrallels_position[index_3];
    }
    
    planet_mesh.fill_empty_field();
}

mesh create_isocaedre()
{
    float phi = (1 + sqrt(5)) / 2.0f; // Nombre d'or
    mesh planet_mesh;

    planet_mesh.position.resize(12);
    planet_mesh.connectivity.resize(20);

    // Ajout des points
    planet_mesh.position[0] = get_point_on_sphere(vec3(-1.0f, phi, 0.0f));
    planet_mesh.position[1] = get_point_on_sphere(vec3(1.0f, phi, 0.0f));
    planet_mesh.position[2] = get_point_on_sphere(vec3(-1.0f, -phi, 0.0f));
    planet_mesh.position[3] = get_point_on_sphere(vec3(1.0f, -phi, 0.0f));
    planet_mesh.position[4] = get_point_on_sphere(vec3(0.0f, -1.0f, phi));
    planet_mesh.position[5] = get_point_on_sphere(vec3(0.0f, 1.0f, phi));
    planet_mesh.position[6] = get_point_on_sphere(vec3(0.0f, -1.0f, -phi));
    planet_mesh.position[7] = get_point_on_sphere(vec3(0.0f, 1.0f, -phi));
    planet_mesh.position[8] = get_point_on_sphere(vec3(phi, 0.0f, -1.0f));
    planet_mesh.position[9] = get_point_on_sphere(vec3(phi, 0.0f, 1.0f));
    planet_mesh.position[10] = get_point_on_sphere(vec3(-phi, 0.0f, -1.0f));
    planet_mesh.position[11] = get_point_on_sphere(vec3(-phi, 0.0f, 1.0f));

    // Triangles de de l'isocaèdre
    planet_mesh.connectivity[0] = uint3(0, 11, 5);
    planet_mesh.connectivity[1] = uint3(0, 5, 1);
    planet_mesh.connectivity[2] = uint3(0, 1, 7);
    planet_mesh.connectivity[3] = uint3(0, 7, 10);
    planet_mesh.connectivity[4] = uint3(0, 10, 11);
    planet_mesh.connectivity[5] = uint3(1, 5, 9);
    planet_mesh.connectivity[6] = uint3(5, 11, 4);
    planet_mesh.connectivity[7] = uint3(11, 10, 2);
    planet_mesh.connectivity[8] = uint3(10, 7, 6);
    planet_mesh.connectivity[9] = uint3(7, 1, 8);
    planet_mesh.connectivity[10] = uint3(3, 9, 4);
    planet_mesh.connectivity[11] = uint3(3, 4, 2);
    planet_mesh.connectivity[12] = uint3(3, 2, 6);
    planet_mesh.connectivity[13] = uint3(3, 6, 8);
    planet_mesh.connectivity[14] = uint3(3, 8, 9);
    planet_mesh.connectivity[15] = uint3(4, 9, 5);
    planet_mesh.connectivity[16] = uint3(2, 4, 11);
    planet_mesh.connectivity[17] = uint3(6, 2, 10);
    planet_mesh.connectivity[18] = uint3(8, 6, 7);
    planet_mesh.connectivity[19] = uint3(9, 8, 1);

    return planet_mesh;
}

vec3 get_point_on_sphere(vec3 position) 
{
    float size = norm(position);
    return position / size * sphere_radius;
}

void curve_mesh(mesh* mesh, float radius) 
{
    float max_norm = 0;
    int number_of_positions = mesh->position.size();
    float _norm;

    for (int i = 0; i < number_of_positions; i++) {
        _norm = norm(mesh->position[i].xy());
        if (_norm > max_norm) max_norm = _norm;
    }
    float new_norm = island_radius / max_norm;

    for (int i = 0; i < number_of_positions; i++) {
        mesh->position[i] *= new_norm;
    }

    for (int i = 0; i < number_of_positions; i++) mesh->position[i].z += sqrt(radius * radius - pow(mesh->position[i].x, 2) - pow(mesh->position[i].y, 2));
}
