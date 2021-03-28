#include "objects.hpp"

using namespace vcl;


Plane::Plane() {
	/*this->plane_timer = timer_interval(0.0, plane_rotation_speed * 2 * pi);*/
	set_visual();
	this->plane_trajectory_visual = trajectory_drawable(1000);
	this->rotation_axis = normalize(vec3(2 * rand_interval() - 1, 2 * rand_interval() - 1, 2 * rand_interval() - 1));
	this->rotation_angle = 0;

	this->plane_trajectory_visual.visual.color = vec3(1, 0, 0);
}

void Plane::set_visual()
{

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

	plane_visual.add(center, "center");
	plane_visual.add(rotated_center, "root", "center");
	plane_visual.add(body, "body", "root", vec3(0, 0, plane_altitude));
	plane_visual.add(nose, "nose", "body", { plane_body_length, 0, 0 });
	plane_visual.add(rear, "rear", "body", { 0, 0, 0 });
	plane_visual.add(left_wing, "left_wing", "body", { plane_body_length * plane_wing_advance_body_proportion, -plane_body_radius, 0 });
	plane_visual.add(right_wing, "right_wing", "body", { plane_body_length * plane_wing_advance_body_proportion, plane_body_radius, 0 });
	plane_visual.add(fin, "fin", "body", { 0, 0, plane_body_radius });
	plane_visual.add(rear_wing_left, "rear_wing_left", "body", { 0, -plane_body_radius, 0 });
	plane_visual.add(rear_wing_right, "rear_wing_right", "body", { 0, plane_body_radius, 0 });


	plane_visual["body"].transform.scale = plane_scale;
	plane_visual["center"].transform.rotate = rotation(rotation_axis, 2 * pi * rand_interval());
	plane_visual.update_local_to_global_coordinates();

	std::cout << plane_visual["body"].transform.translate << std::endl;
}

hierarchy_mesh_drawable& Plane::get_visual() { return plane_visual; }

trajectory_drawable& Plane::get_trajectory_drawable() { return plane_trajectory_visual; }

void Plane::update_and_draw(scene_environement const& scene)
{
	/*plane_timer.update();
	rotation_angle += 2 * pi * plane_rotation_speed;
	plane_visual["root"].transform.rotate = rotation({ 0, 1, 0 }, rotation_angle);
	plane_visual.update_local_to_global_coordinates();*/

	draw(plane_visual, scene);
	draw(plane_trajectory_visual, scene);

	/*plane_trajectory_visual.add(plane_visual["body"].global_transform.translate, plane_timer.t);*/

}

Satelite::Satelite(float _altitude) {
	this->altitude = _altitude;
	this->rotation_axis = normalize(vec3(2 * rand_interval() - 1, 2 * rand_interval() - 1, 2 * rand_interval() - 1));
	this->initial_transform = get_point_at_altitude(cross(orthogonal_axis, rotation_axis));
	this->rotation_angle = 0;
}


hierarchy_mesh_drawable Satelite::get_mesh_drawable()
{
	hierarchy_mesh_drawable result;

	mesh_drawable center = mesh_drawable();
	mesh_drawable rotated_center = mesh_drawable();
	mesh_drawable body = mesh_drawable(mesh_primitive_cube({ 0, 0, 0 }, satelite_body_height));
	mesh_drawable link = mesh_drawable(mesh_primitive_cylinder(satelite_link_radius, { 0, -satelite_link_length / 2, 0 }, { 0, satelite_link_length / 2, 0 }));
	mesh_drawable panel = mesh_drawable(mesh_primitive_quadrangle(
		{ -satelite_panel_height / 2, -satelite_panel_width / 2, 0 },
		{ satelite_panel_height / 2, -satelite_panel_width / 2, 0 },
		{ satelite_panel_height / 2, satelite_panel_width / 2, 0 },
		{ -satelite_panel_height / 2, satelite_panel_width / 2, 0 }
	));

	body.shading.color = vec3(1, 0.9, 0); // Jaune
	link.shading.color = vec3(0, 0, 0); // Noir
	panel.shading.color = vec3(0, 0, 1);

	result.add(center, "center");
	result.add(rotated_center, "root", "center");
	result.add(body, "body", "root", get_point_at_altitude(vec3(0, 0, 1)));
	result.add(link, "left_link", "body", vec3(0, -satelite_body_height / 2 - satelite_link_length / 2, 0));
	result.add(link, "right_link", "body", vec3(0, satelite_body_height / 2 + satelite_link_length / 2, 0));
	result.add(panel, "left_panel", "left_link", vec3(0, -satelite_link_length / 2 - satelite_panel_width / 2, 0));
	result.add(panel, "right_panel", "right_link", vec3(0, satelite_link_length / 2 + satelite_panel_width / 2, 0));

	result["left_panel"].transform.rotate = rotation(vec3(0, 1, 0), pi / 4);
	result["right_panel"].transform.rotate = rotation(vec3(0, 1, 0), pi / 4);

	result["body"].transform.scale = satelite_scale;
	result["center"].transform.rotate = rotation(rotation_axis, 2 * pi * rand_interval());
	result.update_local_to_global_coordinates();

	return result;
}

void Satelite::update_plane_visual(hierarchy_mesh_drawable* plane_visual)
{
	rotation_angle += 2 * pi * satelite_rotation_speed;
	plane_visual->operator[]("root").transform.rotate = rotation({ 0, 1, 0 }, rotation_angle);
	plane_visual->update_local_to_global_coordinates();

}

vcl::vec3 Satelite::get_point_at_altitude(vec3 position)
{
	return position / norm(position) * altitude;
}

void opengl_uniform(GLuint shader, scene_environment const& current_scene)
{
	opengl_uniform(shader, "projection", current_scene.projection);
	opengl_uniform(shader, "view", current_scene.camera.matrix_view());
	opengl_uniform(shader, "light", current_scene.light, false);
}
