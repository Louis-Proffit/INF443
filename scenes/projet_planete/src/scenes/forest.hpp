#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "scene_helper.hpp"
#include "skybox.hpp"

class forest : public scene_visual
{
public:
	vcl::mesh mesh;
	vcl::mesh_drawable visual;

	vcl::camera_around_center camera;

	vcl::mesh_drawable sun_visual;
	skybox skybox;

	// Constructor and destructors;
	forest(user_parameters *user, std::function<void(scene_type)> swap_function);
	~forest();

	// Redefine the virtuals
	void display_visual();
	void update_visual();
	void display_interface();

private:
	perlin_noise_parameters parameters{3, 0.1, 0.3, 2.0};
	float x_min = -2.0f;
	float y_min = -2.0f;
	float x_max = 2.0f;
	float y_max = 2.0f;
	void set_terrain();
	void set_skybox();
	void set_sun();
};