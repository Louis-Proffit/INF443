#include "Sphere.hpp"

using namespace vcl;

mesh_drawable create_sphere();
vec2 get_point_texture_mercator(vec3 position);
vec2 get_point_texture_equicylindrical(vec3 position);
vec3 get_point_on_sphere(vec3 position);
mesh create_isocaedre();

planet::planet() {
    rotation_axis = vec3(0, 0, 1);
    planet_visual = create_sphere();
    rotation_angle = 0;
}

void planet::update(float time)
{
    rotation_angle += 2 * pi * sphere_rotation_speed;
    planet_visual.transform.rotate = rotation(rotation_axis, rotation_angle);
}

mesh_drawable create_sphere()
{
    mesh current_mesh = create_isocaedre();
    unsigned int number_of_triangles, number_of_positions;

    int index_1, index_2, index_3;
    int index_middle_1, index_middle_2, index_middle_3;
    vec3 middle_point_1, middle_point_2, middle_point_3;

    std::map<int, int> middle_points; // Associe le hash_code des deux sommets de l'arrête avec l'indice du sommet du milieu

    for (int i = 0; i < sphere_division_steps; i++) {

        middle_points.clear();
        number_of_positions = current_mesh.position.size();
        number_of_triangles = current_mesh.connectivity.size();

        for (int j = 0; j < number_of_triangles; j++) {

            // Indice des sommets du triangle
            index_1 = current_mesh.connectivity[j][0];
            index_2 = current_mesh.connectivity[j][1];
            index_3 = current_mesh.connectivity[j][2];

            if (middle_points.find(index_1 + number_of_positions * index_2) == middle_points.end()) {
                middle_point_1 = get_point_on_sphere((current_mesh.position[index_1] + current_mesh.position[index_2]) / 2);
                current_mesh.position.push_back(middle_point_1);
                middle_points.insert(std::pair<int, int>(index_1 + number_of_positions * index_2, current_mesh.position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_2 + number_of_positions * index_1, current_mesh.position.size() - 1));
            }
            if (middle_points.find(index_2 + number_of_positions * index_3) == middle_points.end()) {
                middle_point_2 = get_point_on_sphere((current_mesh.position[index_2] + current_mesh.position[index_3]) / 2);
                current_mesh.position.push_back(middle_point_2);
                middle_points.insert(std::pair<int, int>(index_2 + number_of_positions * index_3, current_mesh.position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_3 + number_of_positions * index_2, current_mesh.position.size() - 1));
            }
            if (middle_points.find(index_3 + number_of_positions * index_1) == middle_points.end()) {
                middle_point_3 = get_point_on_sphere((current_mesh.position[index_3] + current_mesh.position[index_1]) / 2);
                current_mesh.position.push_back(middle_point_3);
                middle_points.insert(std::pair<int, int>(index_3 + number_of_positions * index_1, current_mesh.position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_1 + number_of_positions * index_3, current_mesh.position.size() - 1));
            }
        }

        for (int j = 0; j < number_of_triangles; j++) {

            // Indice des sommets du triangle
            index_1 = current_mesh.connectivity[j][0];
            index_2 = current_mesh.connectivity[j][1];
            index_3 = current_mesh.connectivity[j][2];

            // Indice des points milieu du triangle
            index_middle_1 = middle_points.at(index_1 + number_of_positions * index_2);
            index_middle_2 = middle_points.at(index_2 + number_of_positions * index_3);
            index_middle_3 = middle_points.at(index_3 + number_of_positions * index_1);

            current_mesh.connectivity[j] = uint3(index_1, index_middle_1, index_middle_3);
            current_mesh.connectivity.push_back(uint3(index_middle_1, index_2, index_middle_2));
            current_mesh.connectivity.push_back(uint3(index_middle_1, index_middle_2, index_middle_3));
            current_mesh.connectivity.push_back(uint3(index_middle_2, index_3, index_middle_3));
        }
    }

    current_mesh.uv.resize(current_mesh.position.size());
    for (int i = 0; i < current_mesh.position.size(); i++) {
        current_mesh.uv[i] = get_point_texture_equicylindrical(current_mesh.position[i]);
    }

    current_mesh.fill_empty_field();

    mesh_drawable result = mesh_drawable(current_mesh);


    result.shading.phong.specular = 0.0f;
    image_raw earth_surface_texture;
    try
    {
        earth_surface_texture = image_load_png("assets/earth_texture_10k.png");
    }
    catch (const std::exception&)
    {
        earth_surface_texture = image_load_png("../assets/earth_texture_10k.png");
    }
    GLuint const earth_surface_texture_id = opengl_texture_to_gpu(earth_surface_texture, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    result.texture = earth_surface_texture_id;

    return result;
}

vec2 get_point_texture_mercator(vec3 position) {
    float lattitude = pi / 2 - acos(position.z / sphere_radius);
    float longitude = -atan2(position.x, position.y) + pi;


    float u = longitude / (2 * pi);
    float v = 0.15 * log(tan((pi / 4 + lattitude / 2))) + 0.5;

    return vec2(u, v);
}

vec2 get_point_texture_equicylindrical(vec3 position) {
    float lattitude = pi - acos(position.z / sphere_radius);
    float longitude = pi - atan2(position.x, position.y);

    float u = longitude / (2 * pi);
    float v = lattitude / pi;

    return vec2(u, v);
}

vec3 get_point_on_sphere(vec3 position) {
    float size = norm(position);
    return position / size * sphere_radius;
}

mesh create_isocaedre()
{
    mesh result;
    float phi = (1 + sqrt(5)) / 2.0f; // Nombre d'or

    result.position.resize(12);
    result.connectivity.resize(20);

    // Ajout des points
    result.position[0] = get_point_on_sphere(vec3(-1.0f, phi, 0.0f));
    result.position[1] = get_point_on_sphere(vec3(1.0f, phi, 0.0f));
    result.position[2] = get_point_on_sphere(vec3(-1.0f, -phi, 0.0f));
    result.position[3] = get_point_on_sphere(vec3(1.0f, -phi, 0.0f));
    result.position[4] = get_point_on_sphere(vec3(0.0f, -1.0f, phi));
    result.position[5] = get_point_on_sphere(vec3(0.0f, 1.0f, phi));
    result.position[6] = get_point_on_sphere(vec3(0.0f, -1.0f, -phi));
    result.position[7] = get_point_on_sphere(vec3(0.0f, 1.0f, -phi));
    result.position[8] = get_point_on_sphere(vec3(phi, 0.0f, -1.0f));
    result.position[9] = get_point_on_sphere(vec3(phi, 0.0f, 1.0f));
    result.position[10] = get_point_on_sphere(vec3(-phi, 0.0f, -1.0f));
    result.position[11] = get_point_on_sphere(vec3(-phi, 0.0f, 1.0f));

    // Triangles de de l'isocaèdre
    result.connectivity[0] = uint3(0, 11, 5);
    result.connectivity[1] = uint3(0, 5, 1);
    result.connectivity[2] = uint3(0, 1, 7);
    result.connectivity[3] = uint3(0, 7, 10);
    result.connectivity[4] = uint3(0, 10, 11);
    result.connectivity[5] = uint3(1, 5, 9);
    result.connectivity[6] = uint3(5, 11, 4);
    result.connectivity[7] = uint3(11, 10, 2);
    result.connectivity[8] = uint3(10, 7, 6);
    result.connectivity[9] = uint3(7, 1, 8);
    result.connectivity[10] = uint3(3, 9, 4);
    result.connectivity[11] = uint3(3, 4, 2);
    result.connectivity[12] = uint3(3, 2, 6);
    result.connectivity[13] = uint3(3, 6, 8);
    result.connectivity[14] = uint3(3, 8, 9);
    result.connectivity[15] = uint3(4, 9, 5);
    result.connectivity[16] = uint3(2, 4, 11);
    result.connectivity[17] = uint3(6, 2, 10);
    result.connectivity[18] = uint3(8, 6, 7);
    result.connectivity[19] = uint3(9, 8, 1);

    result.fill_empty_field();
    return result;
}

/*mesh create_colored_sphere(vec3 color_1_low, vec3 color_1_high, vec3 color_2_low, vec3 color_2_high) {
    mesh current_sphere = create_sphere();
    const unsigned int number_of_triangles = current_sphere.connectivity.size();

    mesh result;
    vec3 vertex_1, vertex_2, vertex_3;
    unsigned int current_size;
    vec3 triangle_color;

    for (int i = 0; i < number_of_triangles ; i++) {
        vertex_1 = current_sphere.position[current_sphere.connectivity[i][0]];
        vertex_2 = current_sphere.position[current_sphere.connectivity[i][1]];
        vertex_3 = current_sphere.position[current_sphere.connectivity[i][2]];

        triangle_color = generate_random_color((vertex_1 + vertex_2 + vertex_3) / 3, color_1_low, color_1_high, color_2_low, color_2_high);

        current_size = result.position.size();
        result.position.push_back(vertex_1);
        result.position.push_back(vertex_2);
        result.position.push_back(vertex_3);
        result.color.push_back(triangle_color);
        result.color.push_back(triangle_color);
        result.color.push_back(triangle_color);
        result.connectivity.push_back(uint3(current_size, current_size + 1, current_size + 2));
    }

    result.fill_empty_field();

    return result;
    return current_sphere;
}*/

/*mesh shake_sphere(mesh current_sphere) {
    const unsigned int number_of_summits = current_sphere.position.size();
    for (int i = 0; i < number_of_summits; i++) {
        current_sphere.position[i] = get_point_on_sphere(current_sphere.position[i] + perturbation());
    }
    return current_sphere;
}*/

/*vec3 perturbation() {
    const float norm = pow(sphere_radius, 2) / pow(sphere_division_steps, 2);
    return norm * vec3(rand_interval(), rand_interval(), rand_interval());
}*/

/*vec3 generate_random_color(vec3 position, vec3 color_1_low, vec3 color_1_high, vec3 color_2_low, vec3 color_2_high) {
    float random_coefficient_1 = rand_interval();
    float random_coefficient_2 = rand_interval();
    float mix_coefficient = 1 / (1 + exp(position[2] / sphere_radius));
    return mix_coefficient * (random_coefficient_1 * color_1_low + (1 - random_coefficient_1) * color_1_high) + 
        (1 - mix_coefficient) * (random_coefficient_2 * color_2_low + (1 - random_coefficient_2) * color_2_high);
}*/

