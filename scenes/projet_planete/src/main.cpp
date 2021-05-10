#include "vcl/vcl.hpp"
#include <iostream>

#include "scene_helper.hpp"
#include "planet.hpp"

using namespace vcl;

void initialize();
void handle_window_update_callback(GLFWwindow* window, int width, int height);
void handle_mouse_update_callback(GLFWwindow* window, double mouse_x_pos, double mouse_y_pos);

user_parameters user;
scene_visual* scene = 0;

int main(int, char* argv[])
{
	std::cout << "Run " << argv[0] << std::endl;

	int const width = 1280, height = 1024;
	GLFWwindow* window = create_window(width, height);
	imgui_init(window);

	
	std::cout<<"Initialize data ..."<<std::endl;
	initialize();
	scene->handle_window_size_callback(window, width, height);
	glfwSetCursorPosCallback(window, handle_mouse_update_callback);
	glfwSetWindowSizeCallback(window, handle_window_update_callback);

	std::cout<<"Start animation loop ..."<<std::endl;
	user.fps_record.start();
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
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

		if(user.display_frame) draw(user.global_frame, scene);

		scene->display_interface();
		scene->display_visual();

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



void initialize()
{
	/*GLuint const shader_mesh = opengl_create_shader_program(opengl_shader_preset("mesh_vertex"), opengl_shader_preset("mesh_fragment"));
	GLuint const shader_uniform_color = opengl_create_shader_program(opengl_shader_preset("single_color_vertex"), opengl_shader_preset("single_color_fragment"));
	mesh_drawable::default_shader = shader_mesh;
	curve_drawable::default_shader = shader_uniform_color;
	segments_drawable::default_shader = shader_uniform_color;*/
	GLuint const texture_white = opengl_texture_to_gpu(image_raw{ 1,1,image_color_type::rgba,{255,255,255,255} });
	mesh_drawable::default_texture = texture_white;
	mesh_float_drawable::default_texture = texture_white;

	// Paramétrisation de l'utilisateur
	user.global_frame = mesh_drawable(mesh_primitive_frame());
	user.display_frame = false;

	// Initialisation de la planète
	scene = new planet(&user);
}


void handle_window_update_callback(GLFWwindow* window, int width, int height) 
{
	scene->handle_window_size_callback(window, width, height);
}
void handle_mouse_update_callback(GLFWwindow* window, double mouse_x_pos, double mouse_y_pos) 
{
	scene->update_visual(window, mouse_x_pos, mouse_y_pos);
}


