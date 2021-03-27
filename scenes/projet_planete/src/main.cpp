#include "vcl/vcl.hpp"
#include <iostream>

#include "sphere.hpp"
#include "objects.hpp"
#include "constants.hpp"

using namespace vcl;

struct gui_parameters {
	bool display_frame = true;
	bool add_sphere = true;
};

struct user_interaction_parameters {
	vec2 mouse_prev;
	timer_fps fps_record;
	mesh_drawable global_frame;
	gui_parameters gui;
	bool cursor_on_gui;
};
user_interaction_parameters user;

struct scene_environment
{
	camera_around_center camera;
	mat4 projection;
	vec3 light;
};
scene_environment scene;

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void window_size_callback(GLFWwindow* window, int width, int height);

void initialize_data();
void display_scene();
void display_interface();


const vec3 color_blue_low = { float(0) / 255, float(191) / 255, float(255) / 255 };
const vec3 color_blue_high = { float(30) / 255, float(144) / 255, float(255) / 255 };
const vec3 color_grey_low = { float(40) / 255, float(40) / 255, float(40) / 255 };
const vec3 color_grey_high = { float(10) / 255, float(10) / 255, float(10) / 255 };
const vec3 color_green_low = { float(50) / 255, float(205) / 255, float(50) / 255 };
const vec3 color_green_high = { float(34) / 255, float(139) / 255, float(34) / 255 };

timer_interval timer;

mesh_drawable planet;

Plane plane_1(plane_altitude);
hierarchy_mesh_drawable plane_1_visual;
trajectory_drawable plane_1_trajectory(1000);
Plane plane_2(plane_altitude);
hierarchy_mesh_drawable plane_2_visual;
trajectory_drawable plane_2_trajectory(1000);
Satelite satelite_1(satelite_altitude);
hierarchy_mesh_drawable satelite_1_visual;
trajectory_drawable satelite_1_trajectory(2000);
Satelite satelite_2(satelite_altitude);
hierarchy_mesh_drawable satelite_2_visual;
trajectory_drawable satelite_2_trajectory(2000);

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
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
	
	user.global_frame = mesh_drawable(mesh_primitive_frame());
	user.gui.display_frame = false;
	scene.camera.distance_to_center = 2.5f;
	scene.camera.look_at({4,3,2}, {0,0,0}, {0,0,1});

	timer.t_min = 0;
	timer.t_max = 10; 
	timer.t = timer.t_min;

    // Create visual terrain surface
	planet = mesh_drawable(create_colored_sphere(color_blue_low, color_blue_high, color_grey_low, color_grey_high));
	planet.shading.phong.specular = 0.0f;

	image_raw const im = image_load_png("assets/earth_texture_2.png");

	// Send this image to the GPU, and get its identifier texture_image_id
	GLuint const texture_image_id = opengl_texture_to_gpu(im, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	// Associate the texture_image_id to the image texture used when displaying visual
	planet.texture = texture_image_id;

	plane_1_visual = plane_1.get_mesh_drawable();
	plane_2_visual = plane_2.get_mesh_drawable();
	satelite_1_visual = satelite_1.get_mesh_drawable();
	satelite_2_visual = satelite_2.get_mesh_drawable();
}


void display_scene()
{

	timer.update();
	float const t = timer.t;

	draw(planet, scene);


	plane_1_trajectory.visual.color = vec3(1, 0, 0);
	plane_2_trajectory.visual.color = vec3(1, 0, 0);
	satelite_1_trajectory.visual.color = vec3(1, 0, 0);
	satelite_2_trajectory.visual.color = vec3(1, 0, 0);

	plane_1_trajectory.add(plane_1_visual["fin"].global_transform.translate, t);
	plane_2_trajectory.add(plane_2_visual["fin"].global_transform.translate, t);
	satelite_1_trajectory.add(satelite_1_visual["body"].global_transform.translate, t);
	satelite_2_trajectory.add(satelite_2_visual["body"].global_transform.translate, t);

	plane_1.update_plane_visual(&plane_1_visual);
	draw(plane_1_visual, scene);
	draw(plane_1_trajectory, scene);
	plane_2.update_plane_visual(&plane_2_visual);
	draw(plane_2_visual, scene);
	draw(plane_2_trajectory, scene);
	satelite_1.update_plane_visual(&satelite_1_visual);
	draw(satelite_1_visual, scene);
	draw(satelite_1_trajectory, scene);
	satelite_2.update_plane_visual(&satelite_2_visual);
	draw(satelite_2_visual, scene);
	draw(satelite_2_trajectory, scene);
}


void display_interface()
{
	ImGui::Checkbox("Frame", &user.gui.display_frame);
}


void window_size_callback(GLFWwindow* , int width, int height)
{
	glViewport(0, 0, width, height);
	float const aspect = width / static_cast<float>(height);
	scene.projection = projection_perspective(50.0f * pi / 180.0f, aspect, 0.1f, 100.0f);
}


void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	vec2 const  p1 = glfw_get_mouse_cursor(window, xpos, ypos);
	vec2 const& p0 = user.mouse_prev;
	glfw_state state = glfw_current_state(window);

	auto& camera = scene.camera;
	if(!user.cursor_on_gui){
		if(state.mouse_click_left && !state.key_ctrl)
			scene.camera.manipulator_rotate_trackball(p0, p1);
		if(state.mouse_click_left && state.key_ctrl)
			camera.manipulator_translate_in_plane(p1-p0);
		if(state.mouse_click_right)
			camera.manipulator_scale_distance_to_center( (p1-p0).y );
	}

	user.mouse_prev = p1;
}

void opengl_uniform(GLuint shader, scene_environment const& current_scene)
{
	opengl_uniform(shader, "projection", current_scene.projection);
	opengl_uniform(shader, "view", scene.camera.matrix_view());
	opengl_uniform(shader, "light", scene.light, false);
}



