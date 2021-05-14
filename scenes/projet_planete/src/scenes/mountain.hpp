#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "scene_helper.hpp"
#include "skybox.hpp"

class mountain : public scene_visual {
public:
	vcl::mesh mesh;
	vcl::mesh_drawable visual;

	vcl::camera_head camera;

	vcl::mesh_drawable sun_visual;
	skybox skybox;

	// Constructor and destructors;
	mountain(user_parameters* user, std::function<void(scene_type)> swap_function);
	~mountain();

	// Redefine the virtuals
	void display_visual();
	void update_visual();
	void display_interface();

private:
	void set_terrain();
	void set_skybox();
	void set_sun();
};