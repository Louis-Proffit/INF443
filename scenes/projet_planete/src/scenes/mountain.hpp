#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "scene_helper.hpp"

class mountain : public environement {
public:
	vcl::mesh mesh;
	vcl::mesh_drawable visual;

	vcl::mesh_drawable sun_visual;

	// Constructor and destructors;
	mountain(user_parameters* user, std::function<void(scene_type)> swap_function);

	// Redefine the virtuals
	void display_visual();
	void update_visual();
	void display_interface();
	float get_altitude(vcl::vec2 const& position_in_plane);

private:
	typedef environement super;
	void set_terrain();
	void set_skybox();
	void set_sun();
};