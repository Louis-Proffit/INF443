#pragma once

#include "vcl/vcl.hpp"

struct planet {
	vcl::mesh_drawable planet_visual;
	vcl::vec3 rotation_axis;
	float rotation_angle;
	planet(std::string file_name);
	void update(float time);
};

const unsigned int sphere_division_steps = 5;
const float sphere_radius = 1.0f;
const float sphere_rotation_speed = 0.0001f;

/*vcl::vec3 perturbation();
vcl::vec3 generate_random_color(vcl::vec3 position, vcl::vec3 color_1_low, vcl::vec3 color_1_high, vcl::vec3 color_2_low, vcl::vec3 color_2_high);
vcl::mesh create_sphere();
vcl::mesh create_colored_sphere(vcl::vec3 color_1_low, vcl::vec3 color_1_high, vcl::vec3 color_2_low, vcl::vec3 color_2_high);
vcl::mesh shake_sphere(vcl::mesh current_sphere);
vcl::vec3 get_point_on_sphere(vcl::vec3 position);
vcl::mesh create_pyramid();
vcl::mesh create_isocaedre();*/
