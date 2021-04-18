#include "vcl/vcl.hpp"
#include <iostream>

#include "scene_helper.hpp"
#include "orbiters.hpp"
#include "sphere.hpp"
#include "skybox.hpp"

using namespace vcl;

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void window_size_callback(GLFWwindow* window, int width, int height);
void update_and_draw(planet* _planet);
void update_and_draw(orbiter* _orbiter);

void initialize_data();
void display_scene();
void display_interface();

user_interaction_parameters user;
scene_environment scene;
timer_interval timer;

planet* earth;
skybox _skybox;

int number_of_planes = 0;
int number_of_satelites = 0;
std::vector<orbiter*> orbiters;

int main(int, char* argv[])
{
	std::cout << "Run " << argv[0] << std::endl;

	int const width = 1280, height = 1024;
	GLFWwindow* window = create_window(width, height);
	window_size_callback(window, width, height);
	std::cout << opengl_info_display() << std::endl;;

	imgui_init(window);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	
	std::cout<<"Initialize data ..."<<std::endl;
	initialize_data();


	std::cout<<"Start animation loop ..."<<std::endl;
	user.fps_record.start();
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		scene.light = scene.camera.position();

		user.fps_record.update();
		
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		imgui_create_frame();

		if(user.fps_record.event) {
			std::string const title = "VCL Display - "+str(user.fps_record.fps)+" fps";
			glfwSetWindowTitle(window, title.c_str());
		}

		ImGui::Begin("GUI",NULL,ImGuiWindowFlags_AlwaysAutoResize);
		user.cursor_on_gui = ImGui::IsAnyWindowFocused();

		if(user.gui.display_frame) draw(user.global_frame, scene);

		display_interface();
		display_scene();

		ImGui::End();
		imgui_render_frame(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	imgui_cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}



void initialize_data()
{
	GLuint const shader_mesh = opengl_create_shader_program(opengl_shader_preset("mesh_vertex"), opengl_shader_preset("mesh_fragment"));
	GLuint const shader_uniform_color = opengl_create_shader_program(opengl_shader_preset("single_color_vertex"), opengl_shader_preset("single_color_fragment"));
	GLuint const texture_white = opengl_texture_to_gpu(image_raw{1,1,image_color_type::rgba,{255,255,255,255}});
	mesh_drawable::default_shader = shader_mesh;
	mesh_drawable::default_texture = texture_white;
	curve_drawable::default_shader = shader_uniform_color;
	segments_drawable::default_shader = shader_uniform_color;
	
	/* Paramétrisation du user*/
	user.global_frame = mesh_drawable(mesh_primitive_frame());
	user.gui.display_frame = false;
	scene.camera.distance_to_center = 2.5f;
	scene.camera.look_at({ 4,3,2 }, { 0,0,0 }, { 0,0,1 });

	/* Création du timer */
	timer.t_min = 0;
	timer.t_max = 10; 
	timer.t = timer.t_min;

    /* Création de la planète */
	earth = new planet();

	/* Initialisation de l'arrière plan*/
	_skybox.init_skybox(vec3(0, 0, 0), 10, "space");

	/* Initialisation des orbiters */
	for (int i = 0 ; i < number_of_planes ; i++) orbiters.push_back(new orbiter(orbiter_type::PLANE));
	for (int i = 0 ; i < number_of_satelites; i++) orbiters.push_back(new orbiter(orbiter_type::SATELITE));
}


void display_scene()
{
	timer.update();
	float const time = timer.t;

	update_and_draw(earth);

	for (int i = 0; i < orbiters.size(); i++)
		update_and_draw(orbiters[i]);

	_skybox.draw_skybox(scene);
}


void display_interface()
{
	ImGui::Checkbox("Frame", &user.gui.display_frame);
}


void window_size_callback(GLFWwindow* , int width, int height)
{
	glViewport(0, 0, width, height);
	float const aspect = width / static_cast<float>(height);
	float const fov = 50.0f * pi / 180.0f;
	float const z_min = 0.1f;
	float const z_max = 100.0f;
	scene.projection = projection_perspective(fov, aspect, z_min, z_max);
	scene.projection_inverse = projection_perspective_inverse(fov, aspect, z_min, z_max);
}


void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	vec2 const  p1 = glfw_get_mouse_cursor(window, xpos, ypos);
	vec2 const& p0 = user.mouse_prev;
	glfw_state state = glfw_current_state(window);

	auto& camera = scene.camera;
	if (!user.cursor_on_gui && !state.key_shift) {
		if (state.mouse_click_left && !state.key_ctrl)
			scene.camera.manipulator_rotate_trackball(p0, p1);
		if (state.mouse_click_left && state.key_ctrl)
			camera.manipulator_translate_in_plane(p1 - p0);
		if (state.mouse_click_right)
			camera.manipulator_scale_distance_to_center((p1 - p0).y);
	}

	// Handle mouse picking
	picking_position(user.picking, earth->islands_centers, state, scene, p1);

	user.mouse_prev = p1;

}

void update_and_draw(orbiter* _orbiter) {
	_orbiter->update(timer.t);
	_orbiter->trajectory_visual.visual.color.x = 1.0f;
	draw(_orbiter->orbiter_visual, scene);
	draw(_orbiter->trajectory_visual, scene);
}

void update_and_draw(planet* _planet) {
	draw(_planet->planet_visual, scene);
	display_islands(_planet->island_visuals, scene, user.picking);
}


