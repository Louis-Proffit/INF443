#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include "scene_helper.hpp"
#include "water/water.hpp"
#include "water/waterfbuffer.hpp"

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
	std::vector<field> fields;
	std::vector<vcl::mesh> paths;
	std::vector<vcl::mesh_drawable> fields_visuals;
	vcl::mesh_drawable path_visual;
	std::vector<vcl::vec3> tractor_positions;
	vcl::mesh_drawable sand_visual;
	vcl::mesh_drawable tractor_visual;
	vcl::mesh_drawable sun_visual;
	vcl::mesh_drawable draw_fields_type1;
	vcl::mesh_drawable draw_fields_type2;
	vcl::mesh_drawable draw_fields_type3;
	vcl::mesh_drawable draw_fields_type4;

	// Constructor and destructors;
	countryside(user_parameters *user, std::function<void(scene_type)> swap_function);

	// Redefine the virtuals
	void display_visual();
	void update_visual();
	void display_interface();

private:
	typedef environement super;
	float random_compression = 0.3f; // les champs sont r�guliers si le coeff vaut 0, et peuvent �tre triangles si on atteint 1

	float field_min_dimension = 0.3f;
	int field_subdivisions = 10;
	float path_proportion = 0.05f; // proportion du champ sur laquelle on empi�te pour faire un chemin
	float path_dz = 0.005;
	float border_proportion = 0.01f;
	float sand_proportion = 0.5f;
	int number_of_tractors = 3;
	vcl::mesh fields_type1;
	vcl::mesh fields_type2;
	vcl::mesh fields_type3;
	vcl::mesh fields_type4;

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
	void set_assets();
	float get_altitude(vcl::vec2 const &position_in_plane);
	vcl::mesh subdivide_path(vcl::mesh quadrangle);
	vcl::mesh subdivide_field(vcl::mesh quadrangle);
	void shuffle();
	float profile(vcl::vec2 const &position_in_plane);

	// Affichage de l'eau

	void set_water();
	Water wat;
	WaterFrameBuffers fbos;
	vec4 clipPlane = vec4(0, 0, 0, 0);

	void display_scene(vec4 clipPlane);
	void display_reflec_refrac(vec4 clipPlane);
};