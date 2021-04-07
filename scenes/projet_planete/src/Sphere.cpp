#include "Sphere.hpp"

#include <list>
#include <set>

using namespace vcl;

vec3 get_point_on_sphere(vec3 position);
vec3 get_terrain_color(terrain_type _terrain_type);

sphere::sphere() {
    create_sphere();
    set_terrain_type();
    shuffle();
}

planet::planet() {
    rotation_axis = vec3(0, 0, 1);
    rotation_angle = 0;

    /* Configuration du visuel */
    mesh planet_mesh;
    sphere _sphere;
    int number_of_triangles = _sphere.connectivity.size();
    planet_mesh.position.resize(3 * number_of_triangles);
    planet_mesh.color.resize(3 * number_of_triangles);
    planet_mesh.connectivity.resize(number_of_triangles);
    vec3 color;
    for (int i = 0; i < number_of_triangles; i++) {
        planet_mesh.position[3 * i + 0] = _sphere.position[_sphere.connectivity[i][0]];
        planet_mesh.position[3 * i + 1] = _sphere.position[_sphere.connectivity[i][1]];
        planet_mesh.position[3 * i + 2] = _sphere.position[_sphere.connectivity[i][2]];

        planet_mesh.connectivity[i] = uint3(3 * i + 0, 3 * i + 1, 3 * i + 2);

        color = get_terrain_color(_sphere.triangle_terrain_type[i]);

        planet_mesh.color[3 * i + 0] = color;
        planet_mesh.color[3 * i + 1] = color;
        planet_mesh.color[3 * i + 2] = color;
    }

    planet_mesh.fill_empty_field();

    planet_visual = mesh_drawable(planet_mesh);
}

vec3 get_terrain_color(terrain_type _terrain_type) {
    float t = rand_interval();
    switch (_terrain_type) {
    case (terrain_type::SEA):
        return t * color_sea_low + (1 - t) * color_sea_high;
    case (terrain_type::DESERT):
        return t * color_desert_low + (1 - t) * color_desert_high;
    case (terrain_type::MOUNTAIN):
        return t * color_mountain_low + (1 - t) * color_mountain_high;
    case (terrain_type::FIELD):
        return t * color_field_low + (1 - t) * color_field_high;
    case (terrain_type::CITY):
        return t * color_city_low + (1 - t) * color_city_high;
    case (terrain_type::FOREST):
        return t * color_forest_low + (1 - t) * color_forest_high;
    default:
        return vec3(1.0f, 1.0f, 1.0f);
    }
}
void planet::update(float time)
{
    rotation_angle += 2 * pi * sphere_rotation_speed;
    planet_visual.transform.rotate = rotation(rotation_axis, rotation_angle);
}

void sphere::create_sphere() {

    create_isocaedre();

    unsigned int number_of_triangles, number_of_positions;

    int index_1, index_2, index_3;
    int index_middle_1, index_middle_2, index_middle_3;
    vec3 middle_point_1, middle_point_2, middle_point_3;

    std::map<int, int> middle_points; // Associe le hash_code des deux sommets de l'arrête avec l'indice du sommet du milieu

    for (int i = 0; i < sphere_division_steps; i++) {

        middle_points.clear();
        number_of_positions = position.size();
        number_of_triangles = connectivity.size();

        for (int j = 0; j < number_of_triangles; j++) {

            // Indice des sommets du triangle
            index_1 = connectivity[j][0];
            index_2 = connectivity[j][1];
            index_3 = connectivity[j][2];

            if (middle_points.find(index_1 + number_of_positions * index_2) == middle_points.end()) {
                middle_point_1 = get_point_on_sphere((position[index_1] + position[index_2]) / 2);
                position.push_back(middle_point_1);
                middle_points.insert(std::pair<int, int>(index_1 + number_of_positions * index_2, position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_2 + number_of_positions * index_1, position.size() - 1));
            }
            if (middle_points.find(index_2 + number_of_positions * index_3) == middle_points.end()) {
                middle_point_2 = get_point_on_sphere((position[index_2] + position[index_3]) / 2);
                position.push_back(middle_point_2);
                middle_points.insert(std::pair<int, int>(index_2 + number_of_positions * index_3, position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_3 + number_of_positions * index_2, position.size() - 1));
            }
            if (middle_points.find(index_3 + number_of_positions * index_1) == middle_points.end()) {
                middle_point_3 = get_point_on_sphere((position[index_3] + position[index_1]) / 2);
                position.push_back(middle_point_3);
                middle_points.insert(std::pair<int, int>(index_3 + number_of_positions * index_1, position.size() - 1));
                middle_points.insert(std::pair<int, int>(index_1 + number_of_positions * index_3, position.size() - 1));
            }
        }

        for (int j = 0; j < number_of_triangles; j++) {

            // Indice des sommets du triangle
            index_1 = connectivity[j][0];
            index_2 = connectivity[j][1];
            index_3 = connectivity[j][2];

            // Indice des points milieu du triangle
            index_middle_1 = middle_points.at(index_1 + number_of_positions * index_2);
            index_middle_2 = middle_points.at(index_2 + number_of_positions * index_3);
            index_middle_3 = middle_points.at(index_3 + number_of_positions * index_1);

            connectivity[j] = uint3(index_1, index_middle_1, index_middle_3);
            connectivity.push_back(uint3(index_middle_1, index_2, index_middle_2));
            connectivity.push_back(uint3(index_middle_1, index_middle_2, index_middle_3));
            connectivity.push_back(uint3(index_middle_2, index_3, index_middle_3));
        }
    }
}

void sphere::set_terrain_type() 
{
    int number_of_positions = position.size();
    int number_of_triangles = connectivity.size();

    vertex_terrain_type.resize(number_of_positions);
    triangle_terrain_type.resize(number_of_triangles);

    for (int i = 0; i < number_of_positions; i++) vertex_terrain_type[i] = terrain_type::EMPTY;
    for (int i = 0; i < number_of_triangles; i++) triangle_terrain_type[i] = terrain_type::EMPTY;

    /* On crée les sommets initiaux */
    int random_index = -1;

    int counter = 0;
    // Foret
    while (counter < number_of_forest_clusters) {
        random_index = (int)(rand_interval() * number_of_positions);
        if (vertex_terrain_type[random_index] == terrain_type::EMPTY) {
            vertex_terrain_type[random_index] = terrain_type::FOREST;
            counter++;
        }
    }

    // desert
    counter = 0;
    while (counter < number_of_desert_clusters) {
        random_index = (int)(rand_interval() * number_of_positions);
        if (vertex_terrain_type[random_index] == terrain_type::EMPTY) {
            vertex_terrain_type[random_index] = terrain_type::DESERT;
            counter++;
        }
    }

    // ville
    counter = 0;
    while (counter < number_of_city_clusters) {
        random_index = (int)(rand_interval() * number_of_positions);
        if (vertex_terrain_type[random_index] == terrain_type::EMPTY) {
            vertex_terrain_type[random_index] = terrain_type::CITY;
            counter++;
        }
    }

    // campagne
    counter = 0;
    while (counter < number_of_field_clusters) {
        random_index = (int)(rand_interval() * number_of_positions);
        if (vertex_terrain_type[random_index] == terrain_type::EMPTY) {
            vertex_terrain_type[random_index] = terrain_type::FIELD;
            counter++;
        }
    }

    // montagne
    counter = 0;
    while (counter < number_of_mountain_clusters) {
        random_index = (int)(rand_interval() * number_of_positions);
        if (vertex_terrain_type[random_index] == terrain_type::EMPTY) {
            vertex_terrain_type[random_index] = terrain_type::MOUNTAIN;
            counter++;
        }
    }

    // Mer
    counter = 0;
    while (counter < number_of_sea_clusters) {
        random_index = (int)(rand_interval() * number_of_positions);
        if (vertex_terrain_type[random_index] == terrain_type::EMPTY) {
            vertex_terrain_type[random_index] = terrain_type::SEA;
            counter++;
        }
    }

    bool modified = true;
    while (modified) {
        modified = false;
        // Définition des triangles dont un sommet est défini
        for (int i = 0; i < number_of_triangles; i++) {
            if (set_triangle_type(i)) modified = true;
        }
    }

    // Définition des frontières
    for (int i = 0; i < number_of_triangles; i++) {
        set_border_triangle_type(i);
    }

}

void sphere::create_isocaedre()
{
    float phi = (1 + sqrt(5)) / 2.0f; // Nombre d'or

    position.resize(12);
    connectivity.resize(20);

    // Ajout des points
    position[0] = get_point_on_sphere(vec3(-1.0f, phi, 0.0f));
    position[1] = get_point_on_sphere(vec3(1.0f, phi, 0.0f));
    position[2] = get_point_on_sphere(vec3(-1.0f, -phi, 0.0f));
    position[3] = get_point_on_sphere(vec3(1.0f, -phi, 0.0f));
    position[4] = get_point_on_sphere(vec3(0.0f, -1.0f, phi));
    position[5] = get_point_on_sphere(vec3(0.0f, 1.0f, phi));
    position[6] = get_point_on_sphere(vec3(0.0f, -1.0f, -phi));
    position[7] = get_point_on_sphere(vec3(0.0f, 1.0f, -phi));
    position[8] = get_point_on_sphere(vec3(phi, 0.0f, -1.0f));
    position[9] = get_point_on_sphere(vec3(phi, 0.0f, 1.0f));
    position[10] = get_point_on_sphere(vec3(-phi, 0.0f, -1.0f));
    position[11] = get_point_on_sphere(vec3(-phi, 0.0f, 1.0f));

    // Triangles de de l'isocaèdre
    connectivity[0] = uint3(0, 11, 5);
    connectivity[1] = uint3(0, 5, 1);
    connectivity[2] = uint3(0, 1, 7);
    connectivity[3] = uint3(0, 7, 10);
    connectivity[4] = uint3(0, 10, 11);
    connectivity[5] = uint3(1, 5, 9);
    connectivity[6] = uint3(5, 11, 4);
    connectivity[7] = uint3(11, 10, 2);
    connectivity[8] = uint3(10, 7, 6);
    connectivity[9] = uint3(7, 1, 8);
    connectivity[10] = uint3(3, 9, 4);
    connectivity[11] = uint3(3, 4, 2);
    connectivity[12] = uint3(3, 2, 6);
    connectivity[13] = uint3(3, 6, 8);
    connectivity[14] = uint3(3, 8, 9);
    connectivity[15] = uint3(4, 9, 5);
    connectivity[16] = uint3(2, 4, 11);
    connectivity[17] = uint3(6, 2, 10);
    connectivity[18] = uint3(8, 6, 7);
    connectivity[19] = uint3(9, 8, 1);

}

bool sphere::set_triangle_type(int triangle_index) 
{
    // On peut mettre à jour le triangle si il ne l'est pas déja, et si tous les sommets adjascents sont identiques
    uint3 triangle = connectivity[triangle_index];
    terrain_type _terrain_type;

    if (vertex_terrain_type[triangle[0]] == terrain_type::EMPTY && vertex_terrain_type[triangle[1]] == terrain_type::EMPTY && vertex_terrain_type[triangle[2]] == terrain_type::EMPTY) return false;

    if (vertex_terrain_type[triangle[0]] == vertex_terrain_type[triangle[1]] && vertex_terrain_type[triangle[1]] == vertex_terrain_type[triangle[2]] && triangle_terrain_type[triangle_index] == terrain_type::EMPTY) {
        triangle_terrain_type[triangle_index] = vertex_terrain_type[triangle[0]];
        return true;
    }

    // Les trois ne peuvent plus être vides : cas ou deux sont vides
    else if (vertex_terrain_type[triangle[0]] == terrain_type::EMPTY && vertex_terrain_type[triangle[1]] == terrain_type::EMPTY) {
        _terrain_type = vertex_terrain_type[triangle[2]];
        triangle_terrain_type[triangle_index] = _terrain_type;
        vertex_terrain_type[triangle[0]] = _terrain_type;
        vertex_terrain_type[triangle[1]] = _terrain_type;
        return true;
    }
    else if (vertex_terrain_type[triangle[1]] == terrain_type::EMPTY && vertex_terrain_type[triangle[2]] == terrain_type::EMPTY) {
        _terrain_type = vertex_terrain_type[triangle[0]];
        triangle_terrain_type[triangle_index] = _terrain_type;
        vertex_terrain_type[triangle[1]] = _terrain_type;
        vertex_terrain_type[triangle[2]] = _terrain_type;
        return true;
    }
    else if (vertex_terrain_type[triangle[2]] == terrain_type::EMPTY && vertex_terrain_type[triangle[0]] == terrain_type::EMPTY) {
        _terrain_type = vertex_terrain_type[triangle[1]];
        triangle_terrain_type[triangle_index] = _terrain_type;
        vertex_terrain_type[triangle[2]] = _terrain_type;
        vertex_terrain_type[triangle[0]] = _terrain_type;
        return true;
    }

    // Au moins deux sont définis : cas ou il y en a exactement deux et ils sont égaux
    else if (vertex_terrain_type[triangle[0]] == terrain_type::EMPTY && vertex_terrain_type[triangle[1]] == vertex_terrain_type[triangle[2]]) {
        _terrain_type = vertex_terrain_type[triangle[1]];
        triangle_terrain_type[triangle_index] = _terrain_type;
        vertex_terrain_type[triangle[0]] = _terrain_type;
        return true;
    }
    else if (vertex_terrain_type[triangle[1]] == terrain_type::EMPTY && vertex_terrain_type[triangle[2]] == vertex_terrain_type[triangle[0]]) {
        _terrain_type = vertex_terrain_type[triangle[2]];
        triangle_terrain_type[triangle_index] = _terrain_type;
        vertex_terrain_type[triangle[1]] = _terrain_type;
        return true;
    }
    else if (vertex_terrain_type[triangle[2]] == terrain_type::EMPTY && vertex_terrain_type[triangle[0]] == vertex_terrain_type[triangle[1]]) {
        _terrain_type = vertex_terrain_type[triangle[0]];
        triangle_terrain_type[triangle_index] = _terrain_type;
        vertex_terrain_type[triangle[2]] = _terrain_type;
        return true;
    }
    
    // On a au moins deux sommets de couleur différente
    else return false;
}


void sphere::set_border_triangle_type(int triangle_index)
{
    // On peut mettre à jour le triangle si il ne l'est pas déja, et si tous les sommets adjascents sont identiques
    uint3 triangle = connectivity[triangle_index];

    /*// Les trois sommets sont de même couleur
    if (vertex_terrain_type[triangle[0]] == vertex_terrain_type[triangle[1]] && vertex_terrain_type[triangle[1]] == vertex_terrain_type[triangle[2]]) triangle_terrain_type[triangle_index] = vertex_terrain_type[triangle[0]];

    // Deux sommets sont de la même couleur : on choisit cette couleur
    else if (vertex_terrain_type[triangle[0]] == vertex_terrain_type[triangle[1]]) triangle_terrain_type[triangle_index] = vertex_terrain_type[triangle[0]];
    else if (vertex_terrain_type[triangle[1]] == vertex_terrain_type[triangle[2]]) triangle_terrain_type[triangle_index] = vertex_terrain_type[triangle[1]];
    else if (vertex_terrain_type[triangle[2]] == vertex_terrain_type[triangle[0]]) triangle_terrain_type[triangle_index] = vertex_terrain_type[triangle[2]];*/

    // Les trois sommets sont de couleur différente : on en pioche une au hasard
    triangle_terrain_type[triangle_index] = static_cast<terrain_type>(std::min(std::min(vertex_terrain_type[triangle[0]], vertex_terrain_type[triangle[1]]), vertex_terrain_type[triangle[2]]));

}

void sphere::shuffle() {
    float perturbation_proportion = pow(sphere_radius, 2) * float(50) / position.size();
    vec3 vec_unit = vec3(0.5f, 0.5f, 0.5f);
    vec3 perturbation;
    for (int i = 0; i < position.size(); i++)
        position[i] = get_point_on_sphere(position[i] + perturbation_proportion * 2 * (vec3(rand_interval(), rand_interval(), rand_interval()) - vec_unit));
}

vec3 get_point_on_sphere(vec3 position) {
    float size = norm(position);
    return position / size * sphere_radius;
}

