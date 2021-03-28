#pragma once

#include "vcl/vcl.hpp"

struct gui_parameters {
	bool display_frame = true;
	bool add_sphere = true;
};

struct user_interaction_parameters {
	vcl::vec2 mouse_prev;
	vcl::timer_fps fps_record;
	vcl::mesh_drawable global_frame;
	gui_parameters gui;
	bool cursor_on_gui;
};

struct scene_environment
{
	vcl::camera_around_center camera;
	vcl::mat4 projection;
	vcl::mat4 projection_inverse;
	vcl::vec3 light;
};

void opengl_uniform(GLuint shader, scene_environment const& current_scene);

// Scene constants
const float planete_radius = 1.0f;
const float plane_altitude = 1.1f;
const float satelite_altitude = 2.0f;

// Plane graphics
const float plane_body_length = 1.0f;
const float plane_body_radius = 0.1f;
const float plane_nose_length = 0.2f;
const float plane_rear_length = 0.2f;
const float plane_wing_span = 0.8f;
const float plane_wing_inner_length = 0.3f;
const float plane_wing_outer_length = 0.1f;
const float plane_wing_fleche = 0.3f;
const float plane_wing_advance_body_proportion = 0.8f;
const float plane_rear_wing_span = 0.3f;
const float plane_rear_wing_inner_length = 0.2f;
const float plane_rear_wing_outer_length = 0.1f;
const float plane_rear_wing_fleche = 0.15f;
const float plane_fin_span = 0.3f;
const float plane_fin_inner_length = 0.2f;
const float plane_fin_outer_length = 0.1f;
const float plane_fin_fleche = 0.15f;
const float plane_scale = 0.1f;
const float plane_rotation_speed = 0.001f;

// Satelite graphics
const float satelite_body_width = 0.8f;
const float satelite_body_height = 0.8f;
const float satelite_link_length = 0.5f;
const float satelite_link_radius = 0.1f;
const float satelite_panel_height = 1.0f;
const float satelite_panel_width = 3.0f;
const float satelite_scale = 0.01f;
const float satelite_rotation_speed = 0.0001f;

struct scene_environement;

class Plane {

public:
	Plane();
	void update_and_draw(scene_environement const& scene);
	vcl::trajectory_drawable& get_trajectory_drawable();
	vcl::hierarchy_mesh_drawable& get_visual();

private :
	vcl::vec3 rotation_axis;
	vcl::timer_interval plane_timer;
	vcl::hierarchy_mesh_drawable plane_visual;
	vcl::trajectory_drawable plane_trajectory_visual;
	float rotation_angle;
	void set_visual();
};

class Satelite {

public:
	Satelite(float altitude);
	void update_plane_visual(vcl::hierarchy_mesh_drawable* plane_visual);
	vcl::hierarchy_mesh_drawable get_mesh_drawable();

private:
	vcl::vec3 rotation_axis;
	vcl::vec3 orthogonal_axis;
	vcl::vec3 initial_transform;
	float rotation_angle;
	float altitude;
	vcl::vec3 get_point_at_altitude(vcl::vec3 position);
};