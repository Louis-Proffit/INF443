#pragma once

#include "vcl/vcl.hpp"
#include "constants.hpp"

class user_parameters {
public:

	user_parameters();
	~user_parameters();

	vcl::vec2 mouse_prev;
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

	scene_visual(user_parameters* _user);
	~scene_visual();

	virtual void display_visual() = 0;
	virtual void update_visual(GLFWwindow* window, double mouse_x_pos, double mouse_y_pos) = 0;
	virtual void display_interface() = 0;
	void handle_window_size_callback(GLFWwindow* window, int width, int height);
};

std::string open_shader(std::string const& shader_name);