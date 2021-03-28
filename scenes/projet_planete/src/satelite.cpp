#include "objects.hpp"

using namespace vcl;


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
		{-satelite_panel_height / 2, -satelite_panel_width / 2, 0}, 
		{satelite_panel_height / 2, -satelite_panel_width / 2, 0},
		{satelite_panel_height / 2, satelite_panel_width / 2, 0},
		{-satelite_panel_height / 2, satelite_panel_width / 2, 0}
		));

	body.shading.color = vec3(1, 0.9, 0); // Jaune
	link.shading.color = vec3(0, 0, 0); // Noir
	panel.shading.color = vec3(0, 0, 1);

	result.add(center, "center");
	result.add(rotated_center, "root", "center");
	result.add(body, "body", "root", get_point_at_altitude(vec3(0, 0, 1)));
	result.add(link, "left_link", "body", vec3(0, -satelite_body_height / 2 - satelite_link_length / 2, 0));
	result.add(link, "right_link", "body", vec3(0, satelite_body_height / 2 + satelite_link_length / 2, 0));
	result.add(panel, "left_panel", "left_link", vec3(0, - satelite_link_length / 2 - satelite_panel_width / 2, 0));
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

