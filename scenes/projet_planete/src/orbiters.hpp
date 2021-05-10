#pragma once

#include "vcl/vcl.hpp"
#include "constants.hpp"
#include "scene_helper.hpp"

enum class orbiter_type{
	PLANE,
	SATELITE,
	BOAT,
	SUN
};

struct orbiter
{
	vcl::hierarchy_mesh_drawable orbiter_visual;
	vcl::trajectory_drawable trajectory_visual;
	vcl::vec3 rotation_axis;
	float rotation_angle;
	float current_rotate;
	float rotation_speed;

	orbiter(orbiter_type _orbiter_type);
	
	void update(float time);
	void display();
};