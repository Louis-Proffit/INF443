#include "mushroom.hpp"
#include "tree.hpp"
#include "terrain.hpp"

using namespace vcl;

mesh create_mushroom()
{
    float const h = 0.05f; // trunk height
    float const r = 0.04f; // trunk radius

    // Create a brown trunk
    mesh trunk = create_tree_trunk_cylinder(r, h);
    trunk.color.fill({ 1.0f, 1.0f, 1.0f });


    // Create a green foliage from 3 cones
    mesh top_cone = create_cone(2 * r, h, h);      // cone
    top_cone.color.fill({ 1.0f, 0.0f, 0.0f });

    // The tree is composted of the trunk and the foliage
    mesh mushroom = trunk;
    mushroom.push_back(top_cone);

    return mushroom;
}
