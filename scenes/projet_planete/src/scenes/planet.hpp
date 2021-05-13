#pragma once

#include "vcl/vcl.hpp"
#include "constants.hpp"
#include "scene_helper.hpp"
#include "skybox.hpp"

struct picking_structure {
	bool active;
	int index;
};

enum class terrain_type {
	MOUNTAIN,
	DESERT,
	CITY,
	FOREST,
	FIELD
};

class planet : public scene_visual
{
public:
	vcl::mesh planet_mesh;
	vcl::buffer<float> noise;
	vcl::buffer<vcl::vec3> parrallels;
	vcl::buffer<vcl::vec3> islands_centers;
	vcl::buffer<terrain_type> terrain_types;
	vcl::buffer<vcl::mesh_drawable> islands_visuals;
	vcl::mesh_float_drawable planet_visual;

	vcl::mesh sun;
	vcl::mesh_drawable sun_visual;

	vcl::camera_around_center camera;
	skybox skybox;

	// Picking
	picking_structure picking;

	// Constructor and destructors;
	planet(user_parameters* user, std::function<void(scene_type)> swap_function);
	~planet();

	// Redefine the virtuals
	void display_visual();
	void update_visual(vcl::vec2 new_mouse_position);
	void display_interface();

private:
	void create_sphere();
	void set_islands();
	void set_planet();
	void set_skybox();
	void set_sun();
};

vec3 get_point_on_sphere(vec3 position);
mesh create_isocaedre();
void curve_mesh(mesh* mesh, float radius);