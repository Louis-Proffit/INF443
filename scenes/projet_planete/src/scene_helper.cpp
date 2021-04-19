#include "scene_helper.hpp"

using namespace vcl;

void display_islands(buffer<mesh_drawable>& islands, scene_environment const& scene, picking_structure const& picking)
{
	size_t const number_of_islands = islands.size();
	for (size_t i = 0; i < number_of_islands; i++) {
		if (picking.active && picking.index == i)
			islands[i].shading.color = vec3(1, 1, 0);
		else
			islands[i].shading.color = get_default_color(i);
		draw(islands[i], scene);
	}
}

void opengl_uniform(GLuint shader, scene_environment const& current_scene)
{
	opengl_uniform(shader, "projection", current_scene.projection);
	opengl_uniform(shader, "view", current_scene.camera.matrix_view());
	opengl_uniform(shader, "light", current_scene.light, false);
}

void picking_position(picking_structure& picking, buffer<vec3>& islands_centers, glfw_state const& state, scene_environment const& scene, vec2 const& p)
{
	if(state.key_shift)
	{
		if (!state.mouse_click_left)
		{

			vec3 const ray_direction = camera_ray_direction(scene.camera.matrix_frame(), scene.projection_inverse, p);
			vec3 const ray_origin = scene.camera.position();

			intersection_structure intersection = intersection_ray_sphere(ray_origin, ray_direction, { 0, 0, 0 }, sphere_radius);

			if (intersection.valid == true) {
				picking.active = false;
				for (int i = 0; i < islands_centers.size(); i++) {
					if (norm(intersection.position - islands_centers[i]) < island_radius) {
						picking = { true, i };
						break;
					}
				}
			}

			/*picking.active = true;
			vec3 intersection_with_sphere = scene.camera.position() / sphere_radius;
			for (int i = 0; i < islands_centers.size(); i++) {
				if (norm(islands_centers[i] - intersection_with_sphere) < island_radius) {
					picking = { true, i};
					std::cout << "picking " << i << std::endl;
					break;
				}
			}*/
		}

		if (state.mouse_click_left && picking.active)
		{
			// Action lors du clic
			std::cout << "clic sur la surface" << std::endl;
		}
	}
	else
		picking.active = false;
}

vcl::vec3 get_default_color(int index) {
	if (index % 5 == 0) return color_city_low;
	if (index % 5 == 1) return color_desert_low;
	if (index % 5 == 2) return color_field_low;
	if (index % 5 == 3) return color_forest_low;
	if (index % 5 == 4) return color_mountain_low;
}