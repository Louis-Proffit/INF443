#pragma once

#include "vcl/vcl.hpp"
#include "constants.hpp"
#include "scene_helper.hpp"
#include "skybox.hpp"

enum class terrain_type {
	MOUNTAIN,
	DESERT,
	CITY,
	FOREST,
	FIELD
};

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
	vcl::buffer<terrain_type> terrain_types;
	vcl::buffer<vcl::mesh_drawable> islands_visuals;
	vcl::mesh_float_drawable planet_visual;
	vcl::camera_around_center camera;
	skybox skybox;

	// Shaders
	GLuint planet_shader;
	GLuint normal_shader;

	// Picking
	picking_structure picking;

	// Constructor and destructors;
	planet(user_parameters* user);
	~planet();

	// Redefine the virtuals
	void display_visual();
	void update_visual(GLFWwindow* window, double mouse_x_pos, double mouse_y_pos);
	void display_interface();

private:
	void create_sphere();
	void set_islands();
	void set_planet();
	void set_skybox();
};