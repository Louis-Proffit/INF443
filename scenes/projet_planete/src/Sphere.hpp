#pragma once

#include "vcl/vcl.hpp"
#include "constants.hpp"

enum class terrain_type {
	SEA,
	DESERT,
	MOUNTAIN,
	FIELD,
	CITY,
	FOREST,
	EMPTY
};

struct planet 
{
	vcl::mesh planet_mesh;
	vcl::buffer<vcl::vec3> islands_centers;
	vcl::buffer<vcl::mesh_drawable> island_visuals;
	vcl::mesh_drawable planet_visual;

	planet();
	void set_islands();
};
