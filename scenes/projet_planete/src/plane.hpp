#pragma once

#include "vcl/vcl.hpp"

class Plane {

public:
	Plane(float altitude);
	void update_plane_visual(vcl::hierarchy_mesh_drawable *plane_visual);
	vcl::hierarchy_mesh_drawable get_mesh_drawable();

private :
	vcl::vec3 rotation_axis;
	vcl::vec3 orthogonal_axis;
	vcl::vec3 initial_transform;
	float rotation_angle;
	float altitude;
	vcl::vec3 get_point_at_altitude(vcl::vec3 position);
};