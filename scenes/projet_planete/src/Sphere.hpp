#pragma once

#include "vcl/vcl.hpp"
#include "constants.hpp"

enum class terrain_type {
	DESERT,
	MOUNTAIN,
	FIELD,
	CITY,
	FOREST,
	SEA,
	EMPTY
};

struct planet 
{
	vcl::mesh planet_mesh;
	vcl::buffer<vcl::vec3> islands_centers;
	vcl::buffer<terrain_type> terrain_types;
	vcl::buffer<vcl::hierarchy_mesh_drawable> islands_visuals;
	vcl::mesh_drawable planet_visual;
	vcl::vec3 get_terrain_color(int index);

	planet();
	void set_islands();
};
