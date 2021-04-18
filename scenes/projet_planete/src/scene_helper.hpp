#pragma once

#include "vcl/vcl.hpp"

struct scene_environment
{
	vcl::camera_base *camera;
	vcl::mat4 projection;
	vcl::mat4 projection_inverse;
	vcl::vec3 light;
};

struct gui_parameters {
	bool display_frame = false;
	bool camera_around_center = true;
	bool display_polygon = true;
	bool display_keyposition = true;
	bool display_trajectory = true;
	int trajectory_storage = 100;
};

struct user_interaction_parameters {
	vcl::vec2 mouse_prev;
	vcl::timer_fps fps_record;
	vcl::mesh_drawable global_frame;
	gui_parameters gui;
	bool cursor_on_gui;
};

void opengl_uniform(GLuint shader, scene_environment const& current_scene);