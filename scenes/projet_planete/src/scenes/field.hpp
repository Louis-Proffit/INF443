#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "scene_helper.hpp"

enum class field_type
{
	UN,
	DEUX,
	TROIS,
	QUATRE,
	EMPTY
};

struct field
{
	vcl::mesh field_mesh;
	field_type type;

	field(vcl::mesh field_mesh) : field_mesh(field_mesh), type(field_type::EMPTY){};
};

class countryside : public environement
{
public:
	std::vector<field>				fields;
	std::vector<vcl::mesh>			paths;
	std::vector<vcl::mesh_drawable> fields_visuals;
	vcl::mesh_drawable				path_visual;
	std::vector<vcl::vec3>			tractor_positions;
	vcl::mesh_drawable				sand_visual;
	vcl::mesh_drawable				tractor_visual;
	vcl::mesh_drawable				sun_visual;

	// Constructor and destructors;
	countryside(user_parameters* user, std::function<void(scene_type)> swap_function);

	// Redefine the virtuals
	void display_visual();
	void update_visual();
	void display_interface();

private:
	typedef environement super;
	float random_compression = 0.3f; // les champs sont r�guliers si le coeff vaut 0, et peuvent �tre triangles si on atteint 1

	float field_min_dimension = 0.5f;
	int field_subdivisions = 30;
	float path_proportion = 0.05f; // proportion du champ sur laquelle on empi�te pour faire un chemin
	float path_dz = 0.005;
	float border_proportion = 0.01f;
	float sand_proportion = 0.5f;
	int number_of_tractors = 3;

	perlin_noise_parameters parameters{3, 0.1, 0.3, 2.0};

	void set_terrain();
	void set_sand();
	void set_border();
	void set_tractor();
	bool subdivide(int current_subdivisions);
	void set_types();
	void set_textures();
	void set_skybox();
	void set_sun();
	float get_altitude(vcl::vec2 const& position_in_plane);
	vcl::mesh subdivide_path(vcl::mesh quadrangle);
	vcl::mesh subdivide_field(vcl::mesh quadrangle);
	void shuffle();
	float profile(vcl::vec2 const& position_in_plane);
};