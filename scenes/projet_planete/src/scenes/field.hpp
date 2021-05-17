#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "scene_helper.hpp"
#include "skybox.hpp"

enum class field_type {
	UN,
	DEUX,
	TROIS,
	QUATRE,
	CINQ,
	SIX,
	SEPT,
	HUIT,
	NEUF,
	DIX,
	ONZE,
	DOUZE,
	EMPTY
};

struct field {
	vcl::mesh field_mesh;
	field_type type;

	field(vcl::mesh field_mesh) : field_mesh(field_mesh), type(field_type::EMPTY) {};
};

class countryside : public scene_visual {
public:

	std::vector<field> fields;
	std::vector<vcl::mesh> paths;
	std::vector<vcl::mesh_drawable> fields_visuals;
	std::vector<vcl::mesh_drawable> paths_visuals;

	vcl::camera_around_center camera_c;
	vcl::camera_minecraft camera_m;
	bool m_activated;

	vcl::mesh_drawable sun_visual;
	skybox skybox;


	// Constructor and destructors;
	countryside(user_parameters* user, std::function<void(scene_type)> swap_function);
	~countryside();

	// Redefine the virtuals
	void display_visual();
	void update_visual();
	void display_interface();

private:
	float x_min = -2.0f;
	float y_min = -2.0f;
	float x_max = 2.0f;
	float y_max = 2.0f;
	float random_compression = 0.3f; // les champs sont réguliers si le coeff vaut 0, et peuvent être triangles si on atteint 1

	float field_min_dimension = 0.3f;
	int field_subdivisions = 10;
	float path_proportion = 0.05f; // proportion du champ sur laquelle on empiète pour faire un chemin
	float path_z_max = 0.01f;
	float path_z_min = -0.1f;

	perlin_noise_parameters parameters{3, 0.1, 0.3, 2.0};


	void set_terrain();
	bool subdivide(int current_subdivisions);
	void set_types();
	void set_textures();
	void set_skybox();
	void set_sun();
	float get_altitude(vcl::vec2 position_in_plane);
	vcl::mesh subdivide_path(vcl::mesh quadrangle);
	vcl::mesh subdivide_field(vcl::mesh quadrangle);
	void shuffle();
};