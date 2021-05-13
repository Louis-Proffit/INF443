#include "scene_helper.hpp"
#include "../build/assets/shaders/planet/vert.glsl",
#include "../build/assets/shaders/planet/frag.glsl"),
#include "../build/assets/shaders/sun/vert.glsl",
#include "../build/assets/shaders/sun/frag.glsl"),
#include "../build/assets/shaders/normal/vert.glsl",
#include "../build/assets/shaders/normal/frag.glsl");

using namespace vcl;

GLuint scene_visual::planet_shader = 0;
GLuint scene_visual::sun_shader = 0;
GLuint scene_visual::normal_shader = 0;

user_parameters::user_parameters()
{
}

user_parameters::~user_parameters()
{
}

scene_visual::scene_visual(user_parameters* user, std::function<void(scene_type)> _swap_function)
{
	user_reference = user;
	swap_function = _swap_function;
}

scene_visual::~scene_visual()
{
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

	std::cout << "planet shader : " << planet_shader << std::endl;
	std::cout << "sun shader : " << sun_shader << std::endl;
	std::cout << "normal shader : " << normal_shader << std::endl;
}

GLuint scene_visual::open_shader(std::string const& shader_name)
{
	if (shader_name == "planet") return planet_shader;
	else if (shader_name == "sun") return sun_shader;
	else if (shader_name == "normal") return normal_shader;
	
}