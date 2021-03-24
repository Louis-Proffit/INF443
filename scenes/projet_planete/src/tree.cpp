#include "tree.hpp"
#include "terrain.hpp"

using namespace vcl;

mesh create_tree()
{
    float const h = 0.7f; // trunk height
    float const r = 0.1f; // trunk radius

    // Create a brown trunk
    mesh trunk = create_tree_trunk_cylinder(r, h);
    trunk.color.fill({ 0.4f, 0.3f, 0.3f });


    // Create a green foliage from 3 cones
    mesh foliage = create_cone(4 * r, 6 * r, 0.0f);      // base-cone
    foliage.push_back(create_cone(4 * r, 6 * r, 2 * r));   // middle-cone
    foliage.push_back(create_cone(4 * r, 6 * r, 4 * r));   // top-cone
    foliage.position += vec3(0, 0, h);                 // place foliage at the top of the trunk
    foliage.color.fill({ 0.4f, 0.6f, 0.3f });

    // The tree is composted of the trunk and the foliage
    mesh tree = trunk;
    tree.push_back(foliage);

    return tree;
}

mesh create_tree_trunk_cylinder(float radius, float height)
{
    mesh m; 
    const int size = 10;
    float angle;
    float x;
    float y;

    for (int i = 0; i < size; i++) {
        angle = float(i) / size * (2 * pi);
        x = radius * cos(angle);
        y = radius * sin(angle);
        m.position.push_back(vec3(x, y, 0));
        m.position.push_back(vec3(x, y, height));
    }
    for (int i = 0; i < size - 1; i++) {
        m.connectivity.push_back(uint3(2 * i, 2 * i + 1, 2 * i + 2));
        m.connectivity.push_back(uint3(2 * i + 1, 2 * i + 3, 2 * i + 2));
    }

    m.connectivity.push_back(uint3(2 * size - 2, 2 * size - 1, 0));
    m.connectivity.push_back(uint3(2 * size - 1, 1, 0));

    m.fill_empty_field();
    return m;
}

mesh create_cone(float radius, float height, float z_offset)
{
    mesh m;
    const int size = 10;

    m.position.push_back(vec3(0, 0, z_offset));
    m.position.push_back(vec3(0, 0, height + z_offset));
    float angle;

    for (int i = 0; i < size; i++) {
        angle = float(i) / size * (2 * pi);
        m.position.push_back(vec3(radius * cos(angle), radius * sin(angle), z_offset));
    }

    for (int i = 0; i < size - 1; i++) {
        m.connectivity.push_back(uint3(i + 2, i + 3, 1));
        m.connectivity.push_back(uint3(i + 2, i + 3, 0));
    }

    m.connectivity.push_back(uint3(size + 1, 2, 1));
    m.connectivity.push_back(uint3(size + 1, 2, 0));

    m.fill_empty_field();
    return m;
}


