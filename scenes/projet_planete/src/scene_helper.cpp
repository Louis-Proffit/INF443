#include "scene_helper.hpp"
#include "../assets/shaders/planet/vert.glsl"
#include "../assets/shaders/planet/frag.glsl"
#include "../assets/shaders/sun/vert.glsl"
#include "../assets/shaders/sun/frag.glsl"
#include "../assets/shaders/normal/vert.glsl"
#include "../assets/shaders/normal/frag.glsl"
#include "../assets/shaders/heightmap/heightmap.frag.glsl"
#include "../assets/shaders/heightmap/heightmap.vert.glsl"
#include "../assets/shaders/water/water.vert.glsl"
#include "../assets/shaders/water/water.frag.glsl"
#include "../assets/shaders/particles/partic.vert.glsl"
#include "../assets/shaders/particles/partic.frag.glsl"
#include "../assets/shaders/tree/tree.vert.glsl"
#include "../assets/shaders/tree/tree.frag.glsl"

using namespace vcl;

GLuint scene_visual::planet_shader = 0;
GLuint scene_visual::sun_shader = 0;
GLuint scene_visual::normal_shader = 0;
GLuint scene_visual::heightmap_shader = 0;
GLuint scene_visual::water_shader = 0;
GLuint scene_visual::particle_shader = 0;
GLuint scene_visual::tree_shader = 0;

scene_visual::scene_visual(user_parameters *user, std::function<void(scene_type)> _swap_function)
{
	user_reference = user;
	swap_function = _swap_function;
}

void scene_visual::handle_window_size_callback(int width, int height)
{
	glViewport(0, 0, width, height);
	float const aspect = width / static_cast<float>(height);
	float const fov = 50.0f * pi / 180.0f;
	float const z_min = 0.1f;
	float const z_max = 100.0f;
	projection = projection_perspective(fov, aspect, z_min, z_max);
	projection_inverse = projection_perspective_inverse(fov, aspect, z_min, z_max);
}

void scene_visual::init()
{
	scene_visual::planet_shader = opengl_create_shader_program(planet_vert, planet_frag);
	scene_visual::sun_shader = opengl_create_shader_program(sun_vert, sun_frag);
	scene_visual::normal_shader = opengl_create_shader_program(normal_vert, normal_frag);
	scene_visual::heightmap_shader = opengl_create_shader_program(heightmap_vert, heightmap_frag);
	scene_visual::particle_shader = opengl_create_shader_program(partic_vert, partic_frag);
	scene_visual::water_shader = opengl_create_shader_program(water_vert, water_frag);
	scene_visual::tree_shader = opengl_create_shader_program(tree_vertex, tree_fragment);

	std::cout << "planet shader : " << planet_shader << std::endl;
	std::cout << "sun shader : " << sun_shader << std::endl;
	std::cout << "normal shader : " << normal_shader << std::endl;
	std::cout << "heightmap shader : " << heightmap_shader << std::endl;
	std::cout << "particules shader : " << particle_shader << std::endl;
	std::cout << "water shader : " << water_shader << std::endl;
	std::cout << "tree shader : " << tree_shader << std::endl;
}

GLuint scene_visual::open_shader(shader_type shader_type)
{
	switch (shader_type) {
	case shader_type::PLANET:
		return planet_shader;
	case shader_type::SUN:
		return sun_shader;
	case shader_type::NORMAL:
		return normal_shader;
	case shader_type::HEIGHTMAP:
		return heightmap_shader;
	case shader_type::WATER:
		return water_shader;
	case shader_type::PARTICLE:
		return particle_shader;
	case shader_type::TREE:
		return tree_shader;
	}
}

environement::environement(user_parameters* user, std::function<void(scene_type)> swap_function)
	:scene_visual(user, swap_function)
{
	camera_m.position_camera = vec3(0, 0, 0);
	camera_c.distance_to_center = 2.5f;
	camera_c.look_at({ 4, 3, 2 }, { 0, 0, 0 }, { 0, 0, 1 });
	m_activated = true;
}

void environement::display_visual()
{
	skybox.display_skybox(this);
}

void environement::update_visual()
{
	vec2 const& p0 = user_reference->mouse_prev;
	vec2 const& p1 = user_reference->mouse_curr;
	if (m_activated)
	{
		vec2 dp(0, 0);

		if (!user_reference->cursor_on_gui)
		{
			if (user_reference->state.mouse_click_left && !user_reference->state.key_ctrl)
			{
				camera_m.manipulator_rotate_2_axis(p0.y - p1.y, p1.x - p0.x);
			}
		}

		if (user_reference->state.key_up)
			dp.y += 1;
		if (user_reference->state.key_down)
			dp.y -= 1;
		if (user_reference->state.key_left)
			dp.x -= 1;
		if (user_reference->state.key_right)
			dp.x += 1;

		int fps = user_reference->fps_record.fps;
		if (fps <= 0)
			dp *= 0;
		else
			dp = dp *= user_reference->player_speed / fps;

		camera_m.manipulator_set_translation(dp);
		float new_z = get_altitude(camera_m.position_camera.xy());
		camera_m.manipulator_set_altitude(new_z);
	}
	else
	{
		if (!user_reference->cursor_on_gui)
		{
			if (user_reference->state.mouse_click_left && !user_reference->state.key_ctrl)
				camera_c.manipulator_rotate_trackball(p0, p1);
			if (user_reference->state.mouse_click_left && user_reference->state.key_ctrl)
				camera_c.manipulator_translate_in_plane(p1 - p0);
			if (user_reference->state.mouse_click_right)
				camera_c.manipulator_scale_distance_to_center((p1 - p0).y);
		}
	}

	user_reference->mouse_prev = p1;
}

void environement::display_interface()
{
	if (ImGui::Button("Retour maison"))
	{
		swap_function(scene_type::PLANET);
		return;
	}
	if (m_activated)
		m_activated = !ImGui::Button("Camera aerienne");
	else
		m_activated = ImGui::Button("Camera fpv");

	ImGui::Checkbox("Frame", &user_reference->display_frame);
	ImGui::Checkbox("Wireframe", &user_reference->draw_wireframe);
	ImGui::SliderFloat("Vitesse de deplacement", &user_reference->player_speed, 0.1, 2.0f, "%.3f", 2);
}