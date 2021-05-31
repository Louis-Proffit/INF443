#pragma once

#include "vcl/vcl.hpp"
#include "constants.hpp"
#include "scene_helper.hpp"
#include "orbiters.hpp"

struct picking_structure {
	bool active;
	int index;
};

class planet : public scene_visual
{
public:
	vcl::mesh planet_mesh;
	vcl::buffer<float> noise;
	vcl::buffer<vcl::vec3> parrallels;
	vcl::buffer<vcl::vec3> islands_centers;
	vcl::buffer<scene_type> terrain_types;
	vcl::buffer<vcl::mesh_drawable> islands_visuals;
	vcl::mesh_float_drawable planet_visual;

	std::vector<orbiter> orbiters;
	int number_of_planes = 5;
	int number_of_satelites = 3;

	vcl::mesh sun;
	vcl::mesh_drawable sun_visual;

	vcl::camera_around_center camera;

	// Picking
	picking_structure picking;

	// Constructor and destructors;
	planet(user_parameters* user, std::function<void(scene_type)> swap_function);
	~planet();

	// Redefine the virtuals
	void display_visual();
	void update_visual();
	void display_interface();

private:
	void create_sphere();
	void set_islands();
	void set_planet();
	void set_skybox();
	void set_orbiters();
	void set_sun();
};

vcl::vec3 get_point_on_sphere(vcl::vec3 position);
vcl::mesh create_isocaedre();
void curve_mesh(vcl::mesh* mesh, float radius);