#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "scene_helper.hpp"
#include "skybox.hpp"

class desert : public scene_visual{
public:
	vcl::mesh mesh;
	vcl::mesh_drawable visual;

	vcl::camera_head camera;

	vcl::mesh_drawable sun_visual;
	skybox skybox;

	// Constructor and destructors;
	desert(user_parameters* user, std::function<void(scene_type)> swap_function);
	~desert();

	// Redefine the virtuals
	void display_visual();
	void update_visual(vcl::vec2 new_mouse_position);
	void display_interface();

private:
	void set_terrain();
	void set_skybox();
	void set_sun();
};