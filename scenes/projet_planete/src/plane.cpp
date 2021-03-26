#include "plane.hpp"

using namespace vcl;

float body_length = 1.0f;
float body_radius = 0.1f;
float nose_length = 0.2f;
float rear_length = 0.2f;
float wing_span = 0.8f;
float wing_inner_length = 0.3f;
float wing_outer_length = 0.1f;
float wing_fleche = 0.3f;
float wing_advance_body_proportion = 0.8f;
float rear_wing_span = 0.3f;
float rear_wing_inner_length = 0.2f;
float rear_wing_outer_length = 0.1f;
float rear_wing_fleche = 0.15f;
float fin_span = 0.3f;
float fin_inner_length = 0.2f;
float fin_outer_length = 0.1f;
float fin_fleche = 0.15f;
float plane_scale = 0.1f;
float rotation_speed = 0.001;


Plane::Plane(float _altitude) {
	this->altitude = _altitude;
	this->rotation_axis = normalize(vec3(rand_interval(), rand_interval(), rand_interval()));
	this->orthogonal_axis = normalize(vec3(rand_interval(), rand_interval(), rand_interval()));
	this->initial_transform = get_point_at_altitude(cross(orthogonal_axis, rotation_axis));
	this->rotation_angle = 0;
}


hierarchy_mesh_drawable Plane::get_mesh_drawable()
{
	hierarchy_mesh_drawable result;

	mesh_drawable center = mesh_drawable();
	mesh_drawable body = mesh_drawable(mesh_primitive_cylinder(body_radius, { 0, 0, 0 }, { body_length, 0, 0 }));
	mesh_drawable nose = mesh_drawable(mesh_primitive_ellipsoid({ nose_length, body_radius, body_radius }));
	mesh_drawable rear = mesh_drawable(mesh_primitive_ellipsoid({ rear_length, body_radius, body_radius }));
	mesh_drawable left_wing = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { -wing_fleche, -wing_span, 0 }, { -wing_fleche - wing_outer_length, -wing_span, 0 }, { -wing_inner_length, 0, 0 }));
	mesh_drawable right_wing = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { -wing_fleche, wing_span, 0 }, { -wing_fleche - wing_outer_length, wing_span, 0 }, { -wing_inner_length, 0, 0 }));
	mesh_drawable fin = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { fin_inner_length, 0, 0 }, { fin_inner_length - fin_fleche, 0, fin_span }, { fin_inner_length - fin_fleche - fin_outer_length, 0, fin_span }));
	mesh_drawable rear_wing_left = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { rear_wing_inner_length, 0, 0 }, { rear_wing_inner_length - rear_wing_fleche, -rear_wing_span, 0 }, { rear_wing_inner_length - rear_wing_fleche - rear_wing_outer_length, -rear_wing_span, 0 }));
	mesh_drawable rear_wing_right = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { rear_wing_inner_length, 0, 0 }, { rear_wing_inner_length - rear_wing_fleche, rear_wing_span, 0 }, { rear_wing_inner_length - rear_wing_fleche - rear_wing_outer_length, rear_wing_span, 0 }));

	body.shading.color = vec3(0, 0, 1);
	nose.shading.color = vec3(0, 0, 1);
	rear.shading.color = vec3(0, 0, 1);
	left_wing.shading.color = vec3(1, 1, 1);
	right_wing.shading.color = vec3(1, 1, 1);
	fin.shading.color = vec3(1, 1, 1);
	rear_wing_left.shading.color = vec3(1, 1, 1);
	rear_wing_right.shading.color = vec3(1, 1, 1);

	result.add(center, "root");
	result.add(body, "body", "root", initial_transform);
	result.add(nose, "nose", "body", { body_length, 0, 0 });
	result.add(rear, "rear", "body", { 0, 0, 0 });
	result.add(left_wing, "left_wing", "body", { body_length * wing_advance_body_proportion, -body_radius, 0});
	result.add(right_wing, "right_wing", "body", { body_length * wing_advance_body_proportion, body_radius, 0 });
	result.add(fin, "fin", "body", { 0, 0, body_radius });
	result.add(rear_wing_left, "rear_wing_left", "body", { 0, -body_radius, 0 });
	result.add(rear_wing_right, "rear_wing_right", "body", { 0, body_radius, 0 });


	result["body"].transform.scale = plane_scale;
	result["body"].transform.rotate = rotation_between_vector(vec3(1, 0, 0), normalize(cross(rotation_axis, initial_transform)));

	result.update_local_to_global_coordinates();

	return result;
}

void Plane::update_plane_visual(hierarchy_mesh_drawable* plane_visual) 
{
	rotation_angle += 2 * pi * rotation_speed;
	plane_visual->operator[]("root").transform.rotate = rotation(rotation_axis, rotation_angle);
	plane_visual->update_local_to_global_coordinates();

}

vcl::vec3 Plane::get_point_at_altitude(vec3 position)
{
	return position / norm(position) * altitude;
}
