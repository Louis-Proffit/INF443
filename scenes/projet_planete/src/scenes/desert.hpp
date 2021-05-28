#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "scene_helper.hpp"
#include "skybox.hpp"
#include "heightmap.hpp"
#include "water/water.hpp"
#include "water/waterfbuffer.hpp"

class desert : public environement
{
public:
	// Constructor and destructors;
	desert(user_parameters *user, std::function<void(scene_type)> swap_function);

	// Redefine the virtuals
	void display_visual();
	void update_visual();
	void display_interface();

private:
	typedef environement super;
	float horizontal_scale;
	bool height_updated;
	float x_min = -10.0;
	float y_min = -10.0;
	float x_max = 10.0;
	float y_max = 10.0;
	vcl::mesh sun_mesh;
	vcl::mesh terrain_mesh;
	vcl::mesh_drawable terrain_visual;
	vcl::mesh_drawable sun_visual;
	heightmap_parameters parameters;

	std::vector<std::vector<float>> height_data;
	void set_terrain();
	void set_skybox();
	void set_sun();
	void set_water();
	float get_altitude(vcl::vec2 const &position_in_plane);
	float profile(vcl::vec2 const &position_in_plane);

	// Pour l'affichage de l'eau

	Water wat;
	WaterFrameBuffers fbos;
	vec4 clipPlane = vec4(0, 0, 0, 0);

	void display_scene(vec4 clipPlane);
	void display_reflec_refrac(vec4 clipPlane);
};