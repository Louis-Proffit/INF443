#include "sphere.hpp"

#include <list>
#include <set>

using namespace vcl;

vec3 get_point_on_sphere(vec3 position);
mesh create_isocaedre();
mesh create_sphere();
void curve_mesh(mesh *mesh, float radius);


void planet::update(perlin_noise_parameters& parameters)
{
    float max_norm = 0;
    float norm;
    for (int i = 0; i < planet_mesh.position.size(); i++) {
        planet_mesh.position[i] = get_point_on_sphere(planet_mesh.position[i]);
        norm = (1 + parameters.height * noise_perlin(planet_mesh.position[i], parameters.octaves, parameters.persistensy, parameters.frequency_gain));
        planet_mesh.position[i] *= norm;
        if (norm > max_norm) max_norm = norm;
    }
    for (int i = 0; i < planet_mesh.position.size(); i++) planet_mesh.position[i] /= max_norm;
    planet_visual.update_position(planet_mesh.position);
}

void planet::display(scene_environment const& scene, user_interaction_parameters const& user)
{
    draw(planet_visual, scene);
    if (user.draw_wireframe) draw_wireframe(planet_visual, scene);
    for (size_t i = 0; i < number_of_islands; i++) {
        if (user.picking.active && user.picking.index == i)
            islands_visuals[i].shading.color = vec3(1, 0, 0);
        else
            islands_visuals[i].shading.color = vec3(1, 1, 1);
        draw(islands_visuals[i], scene);
    }
 
}

planet::planet() {

    /* Configuration du visuel */
    planet_mesh = create_sphere();
    planet_mesh.fill_empty_field();
    planet_visual = mesh_drawable(planet_mesh);
    planet_visual.shading.color = color_sea_low;
    /*planet_visual.shading.phong = { 0.6 , 0.3, 0, 1 };*/

    set_islands();
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
                if (norm(new_center - islands_centers[j]) < 2 * island_radius) correct = false;
            }
            if (correct) break;
            std::cout << "loop" << std::endl;
        }
        islands_centers[i] = new_center;
        if (i < 5) terrain_types[i] = static_cast<terrain_type>(i);
        else terrain_types[i] = static_cast<terrain_type>(int(rand_interval() * 5));
    }

    for (int i = 0; i < number_of_islands; i++) {
        for (int j = 0; j < number_of_islands; j++) {
            if (i == j) continue;
            assert_vcl(norm(islands_centers[i] - islands_centers[j]) >= 2 * island_radius, "islands too close");
        }
    }

    image_raw texture = image_load_png("assets/textures/lowpoly_palette.png");
    GLuint texture_id = opengl_texture_to_gpu(texture, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    for (int i = 0; i < number_of_islands; i++) {
        mesh_drawable island_visual;
        mesh island_mesh;

        switch (terrain_types[i]) {
        case terrain_type::FOREST :
            island_mesh = mesh_load_file_obj("assets/objects/oak/island_with_oak.obj");
            break;
        case terrain_type::CITY:
            island_mesh = mesh_load_file_obj("assets/objects/house/island_with_house.obj");
            break;
        case terrain_type::DESERT:
            island_mesh = mesh_load_file_obj("assets/objects/palm/island_with_palm.obj");
            break;
        case terrain_type::MOUNTAIN:
            island_mesh = mesh_load_file_obj("assets/objects/mountain/island_with_mountain.obj");
            break;
        case terrain_type::FIELD:
            island_mesh = mesh_load_file_obj("assets/objects/tractor/island_with_tractor.obj");
            break;
        }

        curve_mesh(&island_mesh, 0.99 * sphere_radius);
        island_mesh.fill_empty_field();
        island_visual = mesh_drawable(island_mesh);
        island_visual.texture = texture_id;

        island_visual.transform = affine_rts(rotation_around_center(rotation_between_vector(vec3(0, 0, 1), normalize(islands_centers[i])), vec3(0, 0, 0)));
        islands_visuals.push_back(island_visual);
    }
}

mesh create_sphere() 
{

    mesh planet_mesh = create_isocaedre();

    unsigned int number_of_triangles, number_of_positions;

    int index_1, index_2, index_3;
    int index_middle_1, index_middle_2, index_middle_3;
    vec3 middle_point_1, middle_point_2, middle_point_3;

    std::map<int, int> middle_points; // Associe le hash_code des deux sommets de l'arrête avec l'indice du sommet du milieu

    for (int i = 0; i < sphere_division_steps; i++) {

        middle_points.clear();
        number_of_positions = planet_mesh.position.size();
        number_of_triangles = planet_mesh.connectivity.size();

        for (int j = 0; j < number_of_triangles; j++) {

            // Indice des sommets du triangle
            index_1 = planet_mesh.connectivity[j][0];
            index_2 = planet_mesh.connectivity[j][1];
            index_3 = planet_mesh.connectivity[j][2];

            if (middle_points.find(index_1 + number_of_positions * index_2) == middle_points.end()) {
                middle_point_1 = get_point_on_sphere((planet_mesh.position[index_1] + planet_mesh.position[index_2]) / 2);
                planet_mesh.position.push_back(middle_point_1);
                middle_points.insert(std::pair<int, int>(index_1 + number_of_positions * index_2, planet_mesh.position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_2 + number_of_positions * index_1, planet_mesh.position.size() - 1));
            }
            if (middle_points.find(index_2 + number_of_positions * index_3) == middle_points.end()) {
                middle_point_2 = get_point_on_sphere((planet_mesh.position[index_2] + planet_mesh.position[index_3]) / 2);
                planet_mesh.position.push_back(middle_point_2);
                middle_points.insert(std::pair<int, int>(index_2 + number_of_positions * index_3, planet_mesh.position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_3 + number_of_positions * index_2, planet_mesh.position.size() - 1));
            }
            if (middle_points.find(index_3 + number_of_positions * index_1) == middle_points.end()) {
                middle_point_3 = get_point_on_sphere((planet_mesh.position[index_3] + planet_mesh.position[index_1]) / 2);
                planet_mesh.position.push_back(middle_point_3);
                middle_points.insert(std::pair<int, int>(index_3 + number_of_positions * index_1, planet_mesh.position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_1 + number_of_positions * index_3, planet_mesh.position.size() - 1));
            }
        }

        for (int j = 0; j < number_of_triangles; j++) {

            // Indice des sommets du triangle
            index_1 = planet_mesh.connectivity[j][0];
            index_2 = planet_mesh.connectivity[j][1];
            index_3 = planet_mesh.connectivity[j][2];

            // Indice des points milieu du triangle
            index_middle_1 = middle_points.at(index_1 + number_of_positions * index_2);
            index_middle_2 = middle_points.at(index_2 + number_of_positions * index_3);
            index_middle_3 = middle_points.at(index_3 + number_of_positions * index_1);

            planet_mesh.connectivity[j] = uint3(index_1, index_middle_1, index_middle_3);
            planet_mesh.connectivity.push_back(uint3(index_middle_1, index_2, index_middle_2));
            planet_mesh.connectivity.push_back(uint3(index_middle_1, index_middle_2, index_middle_3));
            planet_mesh.connectivity.push_back(uint3(index_middle_2, index_3, index_middle_3));
        }
    }

    return planet_mesh;
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

