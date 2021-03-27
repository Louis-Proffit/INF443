#include "objects.hpp"
#include "constants.hpp"

using namespace vcl;


Plane::Plane(float _altitude) {
	this->altitude = _altitude;
	this->rotation_axis = normalize(vec3(2 * rand_interval() - 1, 2 * rand_interval() - 1, 2 * rand_interval() - 1));
	this->initial_transform = get_point_at_altitude(cross(orthogonal_axis, rotation_axis));
	this->rotation_angle = 0;
}

hierarchy_mesh_drawable Plane::get_mesh_drawable()
{
	hierarchy_mesh_drawable result;

	mesh_drawable center = mesh_drawable();
	mesh_drawable rotated_center = mesh_drawable();
	mesh_drawable body = mesh_drawable(mesh_primitive_cylinder(plane_body_radius, { 0, 0, 0 }, { plane_body_length, 0, 0 }));
	mesh_drawable nose = mesh_drawable(mesh_primitive_ellipsoid({ plane_nose_length, plane_body_radius, plane_body_radius }));
	mesh_drawable rear = mesh_drawable(mesh_primitive_ellipsoid({ plane_rear_length, plane_body_radius, plane_body_radius }));
	mesh_drawable left_wing = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { -plane_wing_fleche, -plane_wing_span, 0 }, { -plane_wing_fleche - plane_wing_outer_length, -plane_wing_span, 0 }, { -plane_wing_inner_length, 0, 0 }));
	mesh_drawable right_wing = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { -plane_wing_fleche, plane_wing_span, 0 }, { -plane_wing_fleche - plane_wing_outer_length, plane_wing_span, 0 }, { -plane_wing_inner_length, 0, 0 }));
	mesh_drawable fin = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { plane_fin_inner_length, 0, 0 }, { plane_fin_inner_length - plane_fin_fleche, 0, plane_fin_span }, { plane_fin_inner_length - plane_fin_fleche - plane_fin_outer_length, 0, plane_fin_span }));
	mesh_drawable rear_wing_left = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { plane_rear_wing_inner_length, 0, 0 }, { plane_rear_wing_inner_length - plane_rear_wing_fleche, -plane_rear_wing_span, 0 }, { plane_rear_wing_inner_length - plane_rear_wing_fleche - plane_rear_wing_outer_length, -plane_rear_wing_span, 0 }));
	mesh_drawable rear_wing_right = mesh_drawable(mesh_primitive_quadrangle({ 0, 0, 0 }, { plane_rear_wing_inner_length, 0, 0 }, { plane_rear_wing_inner_length - plane_rear_wing_fleche, plane_rear_wing_span, 0 }, { plane_rear_wing_inner_length - plane_rear_wing_fleche - plane_rear_wing_outer_length, plane_rear_wing_span, 0 }));

	body.shading.color = vec3(0, 0, 1);
	nose.shading.color = vec3(0, 0, 1);
	rear.shading.color = vec3(0, 0, 1);
	left_wing.shading.color = vec3(1, 1, 1);
	right_wing.shading.color = vec3(1, 1, 1);
	fin.shading.color = vec3(1, 1, 1);
	rear_wing_left.shading.color = vec3(1, 1, 1);
	rear_wing_right.shading.color = vec3(1, 1, 1);

	result.add(center, "center");
	result.add(rotated_center, "root", "center");
	result.add(body, "body", "root", get_point_at_altitude(vec3(0, 0, 1)));
	result.add(nose, "nose", "body", { plane_body_length, 0, 0 });
	result.add(rear, "rear", "body", { 0, 0, 0 });
	result.add(left_wing, "left_wing", "body", { plane_body_length * plane_wing_advance_body_proportion, -plane_body_radius, 0});
	result.add(right_wing, "right_wing", "body", { plane_body_length * plane_wing_advance_body_proportion, plane_body_radius, 0 });
	result.add(fin, "fin", "body", { 0, 0, plane_body_radius });
	result.add(rear_wing_left, "rear_wing_left", "body", { 0, -plane_body_radius, 0 });
	result.add(rear_wing_right, "rear_wing_right", "body", { 0, plane_body_radius, 0 });


	result["body"].transform.scale = plane_scale;
	result["center"].transform.rotate = rotation(rotation_axis, 2 * pi * rand_interval());
	result.update_local_to_global_coordinates();

	return result;
}

void Plane::update_plane_visual(hierarchy_mesh_drawable* plane_visual) 
{
	rotation_angle += 2 * pi * plane_rotation_speed;
	plane_visual->operator[]("root").transform.rotate = rotation({ 0, 1, 0 }, rotation_angle);
	plane_visual->update_local_to_global_coordinates();

}

vcl::vec3 Plane::get_point_at_altitude(vec3 position)
{
	return position / norm(position) * altitude;
}
