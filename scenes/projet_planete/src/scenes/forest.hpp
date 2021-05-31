#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "helpers/scene_helper.hpp"
#include "helpers/particles.hpp"
#include "L_systems/tree_LSys.hpp"
#include "water/water.hpp"
#include "water/waterfbuffer.hpp"
class forest : public environement
{
public:
	vcl::mesh mesh;
	vcl::mesh_drawable visual;
	vcl::mesh_drawable sun_visual;
	std::vector<tree_located> trees;
	vcl::mesh_drawable rock_visual;
	TreeGenerator tree_cool;
	TreeGenerator tree_classic;
	TreeGenerator tree_real;
	vcl::mesh_float_drawable grass_sol;

	// Constructor and destructors;
	forest(user_parameters *user, std::function<void(scene_type)> swap_function);

	// Redefine the virtuals
	void display_visual();
	void update_visual();
	void display_interface();

private:
	typedef environement super;
	perlin_noise_parameters parameters{3, 0.1, 0.3, 2.0};
	int nb_tree = 100;
	int nb_particles = 20000;
	int N = 300;
	float forest_proportion = 0.6f;
	float profile_transition_down = 0.6f;
	float profile_transition_up = 0.8f;
	float skybox_radius = 10.0f;

	vcl::mesh grass;
	buffer<vcl::vec3> buff_vecgrass;
	buffer<float> buff_floatgrass;
	//float skybox_radius = 10.0f;

	void set_sand();
	void set_terrain();
	void set_skybox();
	void set_sun();
	void set_grass();
	void set_trees();

	std::vector<vcl::vec3> generate_positions_on_terrain(int N);

	float profile(vcl::vec2 const &position_in_plane);
	float get_altitude(vcl::vec2 const &position_in_plane);

	// Water Render
	void set_water();
	Water wat;
	WaterFrameBuffers fbos;
	vec4 clipPlane = vec4(0, 0, 0, 0);

	void display_scene(vec4 clipPlane);
	void display_reflec_refrac(vec4 clipPlane);
};