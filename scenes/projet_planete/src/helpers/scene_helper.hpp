#pragma once

#include <functional>
#include "vcl/vcl.hpp"
#include "constants.hpp"

enum class scene_type
{
	MOUNTAIN,
	DESERT,
	CITY,
	FOREST,
	FIELD,
	PLANET
};

enum class shader_type
{
	PLANET,
	NORMAL,
	SUN,
	HEIGHTMAP,
	WATER,
	PARTICLE,
	TREE,
	MOUNTAIN,
	PARTICLE_MOUNTAIN,
	EBLY,
	FOREST,
	SMOOTH
};

enum class texture_type
{
	FIELD_1,
	FIELD_2,
	FIELD_3,
	FIELD_4,
	SAND,
	GRASS,
	GRASS_ATLAS,
	GRASS_BILLBOARD,
	SNOWFLAKE,
	FIRE,
	LOWPOLY,
	ROCK,
	SNOW,
	FERN2,
	SB_DESERT_HAUT,
	SB_DESERT_BAS,
	SB_DESERT_GAUCHE,
	SB_DESERT_DROITE,
	SB_DESERT_DEVANT,
	SB_DESERT_DERRIERE,
	SB_FLEUVE_HAUT,
	SB_FLEUVE_BAS,
	SB_FLEUVE_GAUCHE,
	SB_FLEUVE_DROITE,
	SB_FLEUVE_DEVANT,
	SB_FLEUVE_DERRIERE,
	SB_NEIGE_HAUT,
	SB_NEIGE_BAS,
	SB_NEIGE_GAUCHE,
	SB_NEIGE_DROITE,
	SB_NEIGE_DEVANT,
	SB_NEIGE_DERRIERE,
	SB_SPACE_HAUT,
	SB_SPACE_BAS,
	SB_SPACE_GAUCHE,
	SB_SPACE_DROITE,
	SB_SPACE_DEVANT,
	SB_SPACE_DERRIERE,
	SB_SUNDOWN_HAUT,
	SB_SUNDOWN_BAS,
	SB_SUNDOWN_GAUCHE,
	SB_SUNDOWN_DROITE,
	SB_SUNDOWN_DEVANT,
	SB_SUNDOWN_DERRIERE
};

enum class skybox_type
{
	DESERT,
	SUNDOWN,
	SPACE,
	NEIGE,
	FLEUVE
};

struct user_parameters
{
	vcl::vec2 mouse_prev;
	vcl::vec2 mouse_curr;
	vcl::timer_fps fps_record;
	vcl::mesh_drawable global_frame;
	vcl::glfw_state state;
	vcl::timer_basic timer;

	bool cursor_on_gui = false;
	bool draw_wireframe = false;
	bool display_frame = false;
	bool sneak = false;
	float player_speed = 0.5f;
};

struct perlin_noise_parameters
{
	int octaves;
	float height;
	float persistency;
	float frequency_gain;
};

class skybox
{
public:
	vcl::mesh_drawable dface_haut;
	vcl::mesh_drawable dface_gauche;
	vcl::mesh_drawable dface_droite;
	vcl::mesh_drawable dface_devant;
	vcl::mesh_drawable dface_derriere;
	vcl::mesh_drawable dface_dessous;

	vcl::vec3 center = vcl::vec3(0, 0, 0);
	float radius = 10.0;

	void init_skybox(vcl::vec3 const &_center, float const &_radius, skybox_type skybox_type, GLuint shader);

	template <typename SCENE>
	void display_skybox(SCENE const &scene)
	{
		draw(dface_dessous, scene);
		draw(dface_devant, scene);
		draw(dface_derriere, scene);
		draw(dface_droite, scene);
		draw(dface_gauche, scene);
		draw(dface_haut, scene);
	}
};

class scene_visual
{
public:
	vcl::mat4 projection;
	vcl::mat4 projection_inverse;
	vcl::vec3 light;
	skybox skybox;
	user_parameters *user_reference;
	std::function<void(scene_type)> swap_function;

	scene_visual(user_parameters *_user, std::function<void(scene_type)> swap_function);
	//virtual ~scene_visual();

	virtual void display_visual() = 0;
	virtual void update_visual() = 0;
	virtual void display_interface() = 0;
	void handle_window_size_callback(int width, int height);

	static void init();

	static GLuint get_shader(shader_type shader_type);
	static GLuint get_texture(texture_type texture_type);
	static GLuint planet_shader;
	static GLuint normal_shader;
	static GLuint sun_shader;
	static GLuint heightmap_shader;
	static GLuint water_shader;
	static GLuint particle_shader;
	static GLuint tree_shader;
	static GLuint mountain_shader;
	static GLuint particle_mountain_shader;
	static GLuint ebly_shader;
	static GLuint forest_shader;
	static GLuint smooth_shader;

	static GLuint texture_field_1;
	static GLuint texture_field_2;
	static GLuint texture_field_3;
	static GLuint texture_field_4;
	static GLuint texture_grass;
	static GLuint texture_grass_atlas;
	static GLuint texture_grass_billboard;
	static GLuint texture_sand;
	static GLuint texture_rock;
	static GLuint texture_fire;
	static GLuint texture_snow;
	static GLuint texture_fern2;
	static GLuint texture_snowflake;
	static GLuint texture_lowpoly;

	static GLuint texture_sb_desert_haut;
	static GLuint texture_sb_desert_bas;
	static GLuint texture_sb_desert_gauche;
	static GLuint texture_sb_desert_droite;
	static GLuint texture_sb_desert_devant;
	static GLuint texture_sb_desert_derriere;
	static GLuint texture_sb_fleuve_haut;
	static GLuint texture_sb_fleuve_bas;
	static GLuint texture_sb_fleuve_gauche;
	static GLuint texture_sb_fleuve_droite;
	static GLuint texture_sb_fleuve_devant;
	static GLuint texture_sb_fleuve_derriere;
	static GLuint texture_sb_neige_haut;
	static GLuint texture_sb_neige_bas;
	static GLuint texture_sb_neige_gauche;
	static GLuint texture_sb_neige_droite;
	static GLuint texture_sb_neige_devant;
	static GLuint texture_sb_neige_derriere;
	static GLuint texture_sb_space_haut;
	static GLuint texture_sb_space_bas;
	static GLuint texture_sb_space_gauche;
	static GLuint texture_sb_space_droite;
	static GLuint texture_sb_space_devant;
	static GLuint texture_sb_space_derriere;
	static GLuint texture_sb_sundown_haut;
	static GLuint texture_sb_sundown_bas;
	static GLuint texture_sb_sundown_gauche;
	static GLuint texture_sb_sundown_droite;
	static GLuint texture_sb_sundown_devant;
	static GLuint texture_sb_sundown_derriere;
};

class environement : public scene_visual
{
public:
	environement(user_parameters *_user, std::function<void(scene_type)> swap_function);

	void display_visual();
	virtual float get_altitude(vcl::vec2 const &position) = 0;
	float get_player_altitude(vcl::vec2 const &position);
	void update_visual();
	void display_interface();

	vcl::camera_around_center camera_c;
	vcl::camera_minecraft camera_m;
	bool m_activated = true;
	float x_min = -10.0;
	float y_min = -10.0;
	float x_max = 10.0;
	float y_max = 10.0;
};
