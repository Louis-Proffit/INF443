#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "scene_helper.hpp"
#include "skybox.hpp"
#include "particles.hpp"
#include "tree_Lsystem/tree_LSys.hpp"

class forest : public scene_visual
{
public:
	vcl::mesh mesh;
	vcl::mesh_drawable visual;

	vcl::camera_around_center camera;

	vcl::mesh_drawable sun_visual;

	ParticleS grass;

	TreeGenerator *trees;

	// Constructor and destructors;
	forest(user_parameters *user, std::function<void(scene_type)> swap_function);
	~forest();

	// Redefine the virtuals
	void display_visual();
	void update_visual();
	void display_interface();

private:
	perlin_noise_parameters parameters{3, 0.1, 0.3, 2.0};
	float x_min = -10.0f;
	float y_min = -10.0f;
	float x_max = 10.0f;
	float y_max = 10.0f;
	int _nbtree = 100;
	void set_terrain();
	void set_skybox();
	void set_sun();
	void set_grass();
	void set_trees(int nbtree);
};