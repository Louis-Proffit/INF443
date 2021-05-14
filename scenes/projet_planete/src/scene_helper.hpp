#pragma once

#include <functional>
#include "vcl/vcl.hpp"
#include "constants.hpp"


enum class scene_type {
	MOUNTAIN,
	DESERT,
	CITY,
	FOREST,
	FIELD,
	PLANET
};

class user_parameters {
public:

	user_parameters();
	~user_parameters();

	vcl::vec2 mouse_prev;
	vcl::vec2 mouse_curr;
	vcl::timer_fps fps_record;
	vcl::mesh_drawable global_frame;
	vcl::glfw_state state;
	vcl::timer_basic timer;

	bool cursor_on_gui;
	bool draw_wireframe;
	bool display_frame;
};

class scene_visual
{
public:

	vcl::mat4 projection;
	vcl::mat4 projection_inverse;
	vcl::vec3 light;
	user_parameters* user_reference;
	std::function<void(scene_type)> swap_function;

	scene_visual(user_parameters* _user, std::function<void(scene_type)> swap_function);
	~scene_visual();

	virtual void display_visual() = 0;
	virtual void update_visual() = 0;
	virtual void display_interface() = 0;
	void handle_window_size_callback(int width, int height);

	static void init();
	static GLuint open_shader(std::string const& shader_name);

private:
	static GLuint planet_shader;
	static GLuint normal_shader;
	static GLuint sun_shader;
};



