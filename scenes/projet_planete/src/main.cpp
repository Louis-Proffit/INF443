#include "vcl/vcl.hpp"
#include <iostream>

#include "scene_helper.hpp"
#include "scenes/city.hpp"
#include "scenes/mountain.hpp"
#include "scenes/desert.hpp"
#include "scenes/forest.hpp"
#include "scenes/field.hpp"
#include "scenes/planet.hpp"

using namespace vcl;

void initialize();

void handle_window_update_callback(GLFWwindow *window, int width, int height);
void handle_mouse_update_callback(GLFWwindow *window, double mouse_x_pos, double mouse_y_pos);
void handle_kb_update_callback(GLFWwindow *window, int i, int j, int k, int l);

user_parameters user;
scene_visual *scene = 0;
GLFWwindow *window = create_window(window_width, window_height);

int main(int, char *argv[])
{
	std::cout << "Run " << argv[0] << std::endl;

	std::cout << "Initialize data ..." << std::endl;
	imgui_init(window);
	initialize();
	handle_window_update_callback(window, window_width, window_height);
	glfwSetCursorPosCallback(window, handle_mouse_update_callback);
	glfwSetKeyCallback(window, handle_kb_update_callback);
	glfwSetWindowSizeCallback(window, handle_window_update_callback);

	std::cout << "Start animation loop ..." << std::endl;
	user.fps_record.start();
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		user.fps_record.update();

		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		imgui_create_frame();

		if (user.fps_record.event)
		{
			std::string const title = "VCL Display - " + str(user.fps_record.fps) + " fps";
			glfwSetWindowTitle(window, title.c_str());
		}
		ImGui::Begin("GUI", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		user.cursor_on_gui = ImGui::IsAnyWindowFocused();

		if (user.display_frame)
			draw(user.global_frame, scene);

		scene->display_interface();
		scene->display_visual();
		scene->update_visual();

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

void swap_function(scene_type scene_type)
{
	switch (scene_type)
	{

	case scene_type::PLANET:
		std::cout << "Swap to planet" << std::endl;
		delete scene;
		scene = new planet(&user, swap_function);
		break;
	case scene_type::CITY:
		std::cout << "Swap to city" << std::endl;
		delete scene;
		scene = new city(&user, swap_function);
		break;
	case scene_type::DESERT:
		std::cout << "Swap to desert" << std::endl;
		delete scene;
		scene = new desert(&user, swap_function);
		break;
	case scene_type::FIELD:
		std::cout << "Swap to field" << std::endl;
		delete scene;
		scene = new countryside(&user, swap_function);
		break;
	case scene_type::FOREST:
		std::cout << "Swap to forest" << std::endl;
		delete scene;
		scene = new forest(&user, swap_function);
		break;
	case scene_type::MOUNTAIN:
		std::cout << "Swap to mountain" << std::endl;
		delete scene;
		scene = new mountain(&user, swap_function);
		break;
	}
	handle_window_update_callback(window, window_width, window_height);
}

void initialize()
{
	/*GLuint const shader_mesh = opengl_create_shader_program(opengl_shader_preset("mesh_vertex"), opengl_shader_preset("mesh_fragment"));
	GLuint const shader_uniform_color = opengl_create_shader_program(opengl_shader_preset("single_color_vertex"), opengl_shader_preset("single_color_fragment"));
	curve_drawable::default_shader = shader_uniform_color;
	segments_drawable::default_shader = shader_uniform_color;*/
	scene_visual::init();
	mesh_drawable::default_shader = scene_visual::open_shader("normal");

	GLuint const texture_white = opengl_texture_to_gpu(image_raw{1, 1, image_color_type::rgba, {255, 255, 255, 255}});
	mesh_drawable::default_texture = texture_white;
	mesh_float_drawable::default_texture = texture_white;

	// Paramï¿½trisation de l'utilisateur
	user.global_frame = mesh_drawable(mesh_primitive_frame(), scene_visual::open_shader("normal"));
	user.display_frame = false;

	// Initialisation de la planète
	scene = new countryside(&user, swap_function);
}

void handle_window_update_callback(GLFWwindow *window, int width, int height)
{
	scene->handle_window_size_callback(width, height);
}
void handle_mouse_update_callback(GLFWwindow *window, double mouse_x_pos, double mouse_y_pos)
{
	user.state = glfw_current_state(window);
	user.mouse_curr = glfw_get_mouse_cursor(window, mouse_x_pos, mouse_y_pos);
	scene->update_visual();
}

void handle_kb_update_callback(GLFWwindow *window, int i, int j, int k, int l)
{
	user.state = glfw_current_state(window);
	user.sneak = user.state.key_shift;
}
