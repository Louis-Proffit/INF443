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
#include "../assets/shaders/particles/partic.mountain.frag.glsl"
#include "../assets/shaders/tree/tree.vert.glsl"
#include "../assets/shaders/tree/tree.frag.glsl"
#include "../assets/shaders/mountain/vert.glsl"
#include "../assets/shaders/mountain/frag.glsl"

using namespace vcl;

GLuint scene_visual::planet_shader = 0;
GLuint scene_visual::sun_shader = 0;
GLuint scene_visual::normal_shader = 0;
GLuint scene_visual::heightmap_shader = 0;
GLuint scene_visual::water_shader = 0;
GLuint scene_visual::particle_shader = 0;
GLuint scene_visual::tree_shader = 0;
GLuint scene_visual::mountain_shader = 0;
GLuint scene_visual::particle_mountain_shader = 0;

GLuint scene_visual::texture_field_1 = 0;
GLuint scene_visual::texture_field_2 = 0;
GLuint scene_visual::texture_field_3 = 0;
GLuint scene_visual::texture_field_4 = 0;
GLuint scene_visual::texture_sand = 0;
GLuint scene_visual::texture_grass = 0;
GLuint scene_visual::texture_grass_atlas = 0;
GLuint scene_visual::texture_grass_billboard = 0;
GLuint scene_visual::texture_fire = 0;
GLuint scene_visual::texture_snowflake = 0;
GLuint scene_visual::texture_lowpoly = 0;
GLuint scene_visual::texture_rock = 0;
GLuint scene_visual::texture_sb_desert_haut = 0;
GLuint scene_visual::texture_sb_desert_bas = 0;
GLuint scene_visual::texture_sb_desert_gauche = 0;
GLuint scene_visual::texture_sb_desert_droite = 0;
GLuint scene_visual::texture_sb_desert_devant = 0;
GLuint scene_visual::texture_sb_desert_derriere = 0;
GLuint scene_visual::texture_sb_fleuve_haut = 0;
GLuint scene_visual::texture_sb_fleuve_bas = 0;
GLuint scene_visual::texture_sb_fleuve_gauche = 0;
GLuint scene_visual::texture_sb_fleuve_droite = 0;
GLuint scene_visual::texture_sb_fleuve_devant = 0;
GLuint scene_visual::texture_sb_fleuve_derriere = 0;
GLuint scene_visual::texture_sb_neige_haut = 0;
GLuint scene_visual::texture_sb_neige_bas = 0;
GLuint scene_visual::texture_sb_neige_gauche = 0;
GLuint scene_visual::texture_sb_neige_droite = 0;
GLuint scene_visual::texture_sb_neige_devant = 0;
GLuint scene_visual::texture_sb_neige_derriere = 0;
GLuint scene_visual::texture_sb_space_haut = 0;
GLuint scene_visual::texture_sb_space_bas = 0;
GLuint scene_visual::texture_sb_space_gauche = 0;
GLuint scene_visual::texture_sb_space_droite = 0;
GLuint scene_visual::texture_sb_space_devant = 0;
GLuint scene_visual::texture_sb_space_derriere = 0;
GLuint scene_visual::texture_sb_sundown_haut = 0;
GLuint scene_visual::texture_sb_sundown_bas = 0;
GLuint scene_visual::texture_sb_sundown_gauche = 0;
GLuint scene_visual::texture_sb_sundown_droite = 0;
GLuint scene_visual::texture_sb_sundown_devant = 0;
GLuint scene_visual::texture_sb_sundown_derriere = 0;

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
	scene_visual::mountain_shader = opengl_create_shader_program(mountain_vert, mountain_frag);
	scene_visual::particle_mountain_shader = opengl_create_shader_program(partic_vert, partic_mountain_frag);

	scene_visual::texture_field_1 = opengl_texture_to_gpu(image_load_png("../assets/textures/field/field_1.png"), GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
	scene_visual::texture_field_2 = opengl_texture_to_gpu(image_load_png("../assets/textures/field/field_2.png"), GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
	scene_visual::texture_field_3 = opengl_texture_to_gpu(image_load_png("../assets/textures/field/field_3.png"), GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
	scene_visual::texture_field_4 = opengl_texture_to_gpu(image_load_png("../assets/textures/field/field_4.png"), GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
	scene_visual::texture_grass_billboard = opengl_texture_to_gpu(image_load_png("../assets/textures/grass/grass_billboard.png"), GL_REPEAT, GL_REPEAT);
	scene_visual::texture_grass = opengl_texture_to_gpu(image_load_png("../assets/textures/grass/grass_1.png"), GL_REPEAT, GL_REPEAT);
	scene_visual::texture_grass_atlas = opengl_texture_to_gpu(image_load_png("../assets/textures/grass/grass_atlas.png"), GL_REPEAT, GL_REPEAT);
	scene_visual::texture_rock = opengl_texture_to_gpu(image_load_png("../assets/textures/mountain/rock.png"), GL_REPEAT, GL_REPEAT);
	scene_visual::texture_sand = opengl_texture_to_gpu(image_load_png("../assets/textures/desert/sand_1.png"), GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
	scene_visual::texture_fire = opengl_texture_to_gpu(image_load_png("../assets/textures/fire/fire.png"), GL_REPEAT, GL_REPEAT);
	scene_visual::texture_snowflake = opengl_texture_to_gpu(image_load_png("../assets/textures/snow/snowflake_3.png"), GL_REPEAT, GL_REPEAT);
	scene_visual::texture_lowpoly = opengl_texture_to_gpu(image_load_png("../assets/textures/lowpoly_palette.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_desert_haut = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/desert/top.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_desert_bas = opengl_texture_to_gpu(image_raw{1, 1, image_color_type::rgba, {255, 255, 255, 255}});
	scene_visual::texture_sb_desert_gauche = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/desert/left.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_desert_droite = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/desert/right.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_desert_devant = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/desert/front.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_desert_derriere = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/desert/back.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_fleuve_haut = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/fleuve/top.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_fleuve_bas = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/fleuve/bottom.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_fleuve_gauche = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/fleuve/left.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_fleuve_droite = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/fleuve/right.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_fleuve_devant = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/fleuve/front.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_fleuve_derriere = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/fleuve/back.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_neige_haut = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/neige/top.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_neige_bas = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/neige/bottom.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_neige_gauche = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/neige/left.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_neige_droite = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/neige/right.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_neige_devant = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/neige/front.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_neige_derriere = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/neige/back.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_space_haut = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/space/top.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_space_bas = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/space/bottom.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_space_gauche = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/space/left.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_space_droite = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/space/right.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_space_devant = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/space/front.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_space_derriere = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/space/back.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_sundown_haut = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/sundown/top.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_sundown_bas = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/sundown/bottom.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_sundown_gauche = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/sundown/left.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_sundown_droite = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/sundown/right.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_sundown_devant = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/sundown/front.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	scene_visual::texture_sb_sundown_derriere = opengl_texture_to_gpu(image_load_png("../assets/skyboxes/sundown/back.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

GLuint scene_visual::get_shader(shader_type shader_type)
{
	switch (shader_type)
	{
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
	case shader_type::MOUNTAIN:
		return mountain_shader;
	case shader_type::PARTICLE_MOUNTAIN:
		return particle_mountain_shader;
	}
}

GLuint scene_visual::get_texture(texture_type texture_type)
{
	switch (texture_type)
	{
	case texture_type::FIELD_1:
		return texture_field_1;
	case texture_type::FIELD_2:
		return texture_field_2;
	case texture_type::FIELD_3:
		return texture_field_3;
	case texture_type::FIELD_4:
		return texture_field_4;
	case texture_type::SAND:
		return texture_sand;
	case texture_type::GRASS:
		return texture_grass;
	case texture_type::GRASS_ATLAS:
		return texture_grass_atlas;
	case texture_type::GRASS_BILLBOARD:
		return texture_grass_billboard;
	case texture_type::SNOWFLAKE:
		return texture_snowflake;
	case texture_type::FIRE:
		return texture_fire;
	case texture_type::LOWPOLY:
		return texture_lowpoly;
	case texture_type::ROCK:
		return texture_rock;
	case texture_type::SB_DESERT_HAUT:
		return texture_sb_desert_haut;
	case texture_type::SB_DESERT_BAS:
		return texture_sb_desert_bas;
	case texture_type::SB_DESERT_GAUCHE:
		return texture_sb_desert_gauche;
	case texture_type::SB_DESERT_DROITE:
		return texture_sb_desert_droite;
	case texture_type::SB_DESERT_DEVANT:
		return texture_sb_desert_devant;
	case texture_type::SB_DESERT_DERRIERE:
		return texture_sb_desert_derriere;
	case texture_type::SB_NEIGE_HAUT:
		return texture_sb_neige_haut;
	case texture_type::SB_NEIGE_BAS:
		return texture_sb_neige_bas;
	case texture_type::SB_NEIGE_GAUCHE:
		return texture_sb_neige_gauche;
	case texture_type::SB_NEIGE_DROITE:
		return texture_sb_neige_droite;
	case texture_type::SB_NEIGE_DEVANT:
		return texture_sb_neige_devant;
	case texture_type::SB_NEIGE_DERRIERE:
		return texture_sb_neige_derriere;
	case texture_type::SB_SPACE_HAUT:
		return texture_sb_space_haut;
	case texture_type::SB_SPACE_BAS:
		return texture_sb_space_bas;
	case texture_type::SB_SPACE_GAUCHE:
		return texture_sb_space_gauche;
	case texture_type::SB_SPACE_DROITE:
		return texture_sb_space_droite;
	case texture_type::SB_SPACE_DEVANT:
		return texture_sb_space_devant;
	case texture_type::SB_SPACE_DERRIERE:
		return texture_sb_space_derriere;
	case texture_type::SB_SUNDOWN_HAUT:
		return texture_sb_sundown_haut;
	case texture_type::SB_SUNDOWN_BAS:
		return texture_sb_sundown_bas;
	case texture_type::SB_SUNDOWN_GAUCHE:
		return texture_sb_sundown_gauche;
	case texture_type::SB_SUNDOWN_DROITE:
		return texture_sb_sundown_droite;
	case texture_type::SB_SUNDOWN_DEVANT:
		return texture_sb_sundown_devant;
	case texture_type::SB_SUNDOWN_DERRIERE:
		return texture_sb_sundown_derriere;
	case texture_type::SB_FLEUVE_HAUT:
		return texture_sb_fleuve_haut;
	case texture_type::SB_FLEUVE_BAS:
		return texture_sb_fleuve_bas;
	case texture_type::SB_FLEUVE_GAUCHE:
		return texture_sb_fleuve_gauche;
	case texture_type::SB_FLEUVE_DROITE:
		return texture_sb_fleuve_droite;
	case texture_type::SB_FLEUVE_DEVANT:
		return texture_sb_fleuve_devant;
	case texture_type::SB_FLEUVE_DERRIERE:
		return texture_sb_fleuve_derriere;
	}
}

environement::environement(user_parameters *user, std::function<void(scene_type)> swap_function)
	: scene_visual(user, swap_function)
{
	camera_m.position_camera = vec3(0, 0, 0);
	camera_c.distance_to_center = 2.5f;
	camera_c.look_at({4, 3, 2}, {0, 0, 0}, {0, 0, 1});
	m_activated = true;
}

void environement::display_visual()
{
	skybox.display_skybox(this);
}

void environement::update_visual()
{
	vec2 const &p0 = user_reference->mouse_prev;
	vec2 const &p1 = user_reference->mouse_curr;
	if (m_activated)
	{
		vec2 dp(0, 0);

		if (!user_reference->cursor_on_gui)
		{
			if (user_reference->state.mouse_click_left && !user_reference->state.key_ctrl)
			{
				camera_m.manipulator_rotate_2_axis(p1.y - p0.y, p1.x - p0.x);
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

void skybox::init_skybox(vec3 const &_center, float const &_radius, skybox_type skybox_type, GLuint shader)
{
	this->center = _center;
	this->radius = _radius;
	GLuint derriere, devant, gauche, droite, haut, bas;
	switch (skybox_type)
	{
	case skybox_type::DESERT:
		haut = scene_visual::get_texture(texture_type::SB_DESERT_HAUT);
		bas = scene_visual::get_texture(texture_type::SB_DESERT_BAS);
		gauche = scene_visual::get_texture(texture_type::SB_DESERT_GAUCHE);
		droite = scene_visual::get_texture(texture_type::SB_DESERT_DROITE);
		devant = scene_visual::get_texture(texture_type::SB_DESERT_DEVANT);
		derriere = scene_visual::get_texture(texture_type::SB_DESERT_DERRIERE);
		break;
	case skybox_type::FLEUVE:
		haut = scene_visual::get_texture(texture_type::SB_FLEUVE_HAUT);
		bas = scene_visual::get_texture(texture_type::SB_FLEUVE_BAS);
		gauche = scene_visual::get_texture(texture_type::SB_FLEUVE_GAUCHE);
		droite = scene_visual::get_texture(texture_type::SB_FLEUVE_DROITE);
		devant = scene_visual::get_texture(texture_type::SB_FLEUVE_DEVANT);
		derriere = scene_visual::get_texture(texture_type::SB_FLEUVE_DERRIERE);
		break;
	case skybox_type::NEIGE:
		haut = scene_visual::get_texture(texture_type::SB_NEIGE_HAUT);
		bas = scene_visual::get_texture(texture_type::SB_NEIGE_BAS);
		gauche = scene_visual::get_texture(texture_type::SB_NEIGE_GAUCHE);
		droite = scene_visual::get_texture(texture_type::SB_NEIGE_DROITE);
		devant = scene_visual::get_texture(texture_type::SB_NEIGE_DEVANT);
		derriere = scene_visual::get_texture(texture_type::SB_NEIGE_DERRIERE);
		break;
	case skybox_type::SPACE:
		haut = scene_visual::get_texture(texture_type::SB_SPACE_HAUT);
		bas = scene_visual::get_texture(texture_type::SB_SPACE_BAS);
		gauche = scene_visual::get_texture(texture_type::SB_SPACE_GAUCHE);
		droite = scene_visual::get_texture(texture_type::SB_SPACE_DROITE);
		devant = scene_visual::get_texture(texture_type::SB_SPACE_DEVANT);
		derriere = scene_visual::get_texture(texture_type::SB_SPACE_DERRIERE);
		break;
	case skybox_type::SUNDOWN:
		haut = scene_visual::get_texture(texture_type::SB_SUNDOWN_HAUT);
		bas = scene_visual::get_texture(texture_type::SB_SUNDOWN_BAS);
		gauche = scene_visual::get_texture(texture_type::SB_SUNDOWN_GAUCHE);
		droite = scene_visual::get_texture(texture_type::SB_SUNDOWN_DROITE);
		devant = scene_visual::get_texture(texture_type::SB_SUNDOWN_DEVANT);
		derriere = scene_visual::get_texture(texture_type::SB_SUNDOWN_DERRIERE);
		break;
	}

	// front face
	mesh face_devant = mesh_primitive_grid(vec3(-radius, -radius, radius), vec3(-radius, -radius, -radius), vec3(-radius, radius, -radius), vec3(-radius, radius, radius), 2, 2);
	dface_devant = mesh_drawable(face_devant, shader);
	dface_devant.texture = devant;
	dface_devant.shading.phong = {1, 0, 0, 1};

	//back face
	mesh face_derriere = mesh_primitive_grid(vec3(radius, radius, radius), vec3(radius, radius, -radius), vec3(radius, -radius, -radius), vec3(radius, -radius, radius), 2, 2);
	dface_derriere = mesh_drawable(face_derriere, shader);
	dface_derriere.texture = derriere;
	dface_derriere.shading.phong = {1, 0, 0, 1};

	//face droite
	mesh face_droite = mesh_primitive_grid(vec3(-radius, radius, radius), vec3(-radius, radius, -radius), vec3(radius, radius, -radius), vec3(radius, radius, radius), 2, 2);
	dface_droite = mesh_drawable(face_droite, shader);
	dface_droite.texture = droite;
	dface_droite.shading.phong = {1, 0, 0, 1};

	//face gauche
	mesh face_gauche = mesh_primitive_grid(vec3(radius, -radius, radius), vec3(radius, -radius, -radius), vec3(-radius, -radius, -radius), vec3(-radius, -radius, radius), 2, 2);
	dface_gauche = mesh_drawable(face_gauche, shader);
	dface_gauche.texture = gauche;
	dface_gauche.shading.phong = {1, 0, 0, 1};

	//dessus
	mesh face_haut = mesh_primitive_grid(vec3(radius, -radius, radius), vec3(-radius, -radius, radius), vec3(-radius, radius, radius), vec3(radius, radius, radius), 2, 2);
	dface_haut = mesh_drawable(face_haut, shader);
	dface_haut.texture = haut;
	dface_haut.shading.phong = {1, 0, 0, 1};

	mesh face_dessous = mesh_primitive_grid(vec3(radius, radius, -radius), vec3(-radius, radius, -radius), vec3(-radius, -radius, -radius), vec3(radius, -radius, -radius), 2, 2);
	dface_dessous = mesh_drawable(face_dessous, shader);
	dface_dessous.texture = bas;
	dface_dessous.shading.phong = {1, 0, 0, 1};
}