#pragma once

#include "vcl/vcl.hpp"
#include "constants.hpp"

enum class terrain_type {
	SEA,
	DESERT,
	MOUNTAIN,
	FIELD,
	CITY,
	FOREST,
	EMPTY
};

struct sphere {

	const int number_of_sea_clusters = 20;
	const int number_of_desert_clusters = 15;
	const int number_of_mountain_clusters = 12;
	const int number_of_field_clusters = 10;
	const int number_of_city_clusters = 15;
	const int number_of_forest_clusters = 18;

	vcl::buffer<vcl::vec3> position;
	vcl::buffer<vcl::uint3> connectivity;
	vcl::buffer<terrain_type> vertex_terrain_type;
	vcl::buffer<terrain_type> triangle_terrain_type;
	sphere();

private:
	void shuffle();
	void create_sphere();
	void create_isocaedre();
	bool set_triangle_type(int triangle_index);
	void set_border_triangle_type(int triangle_index);
	void set_terrain_type();
};

struct planet {
	vcl::mesh_drawable planet_visual;
	vcl::vec3 rotation_axis;
	float rotation_angle;
	planet();
	void update(float time);
};

/*vcl::vec3 perturbation();
vcl::vec3 generate_random_color(vcl::vec3 position, vcl::vec3 color_1_low, vcl::vec3 color_1_high, vcl::vec3 color_2_low, vcl::vec3 color_2_high);
vcl::mesh create_sphere();
vcl::mesh create_colored_sphere(vcl::vec3 color_1_low, vcl::vec3 color_1_high, vcl::vec3 color_2_low, vcl::vec3 color_2_high);
vcl::mesh shake_sphere(vcl::mesh current_sphere);
vcl::vec3 get_point_on_sphere(vcl::vec3 position);
vcl::mesh create_pyramid();
vcl::mesh create_isocaedre();*/
