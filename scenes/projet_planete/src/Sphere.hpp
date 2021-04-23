#pragma once

#include "vcl/vcl.hpp"
#include "constants.hpp"
#include "scene_helper.hpp"

enum class terrain_type {
	MOUNTAIN,
	DESERT,
	CITY,
	FOREST,
	FIELD
};

struct planet 
{
	vcl::mesh planet_mesh;
	vcl::buffer<vcl::vec3> islands_centers;
	vcl::buffer<terrain_type> terrain_types;
	vcl::buffer<vcl::mesh_drawable> islands_visuals;
	vcl::mesh_drawable planet_visual;
	void update(perlin_noise_parameters& _paramaters);
	void display(scene_environment const& scene, user_interaction_parameters const& user);
	void set_islands();
	planet();
};
