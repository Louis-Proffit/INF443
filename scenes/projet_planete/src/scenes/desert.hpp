#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "scene_helper.hpp"
#include "skybox.hpp"
#include "heightmap.hpp"

class desert : public scene_visual{
public:

	// Constructor and destructors;
	desert(user_parameters* user, std::function<void(scene_type)> swap_function);
	~desert();

	// Redefine the virtuals
	void display_visual();
	void update_visual();
	void display_interface();

private:
	float horizontal_scale;
	bool height_updated;

	vcl::camera_minecraft		camera_m;
	vcl::camera_around_center	camera_c;
	bool						m_activated;
	vcl::mesh					sun_mesh;
	vcl::mesh					mesh;
	vcl::mesh_drawable			visual;
	vcl::mesh_drawable			sun_visual;
	heightmap_parameters		parameters;
	skybox						skybox;

	std::vector<std::vector<float> > height_data;
	void set_terrain();
	void set_skybox();
	void set_sun();
	float get_altitude(vcl::vec2 const& position_in_plane);
};