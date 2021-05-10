#include "scene_helper.hpp"

using namespace vcl;

user_parameters::user_parameters()
{
}

user_parameters::~user_parameters()
{
}

scene_visual::scene_visual(user_parameters* user)
{
	user_reference = user;
}

scene_visual::~scene_visual()
{
}

void scene_visual::handle_window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	float const aspect = width / static_cast<float>(height);
	float const fov = 50.0f * pi / 180.0f;
	float const z_min = 0.1f;
	float const z_max = 100.0f;
	projection = projection_perspective(fov, aspect, z_min, z_max);
	projection_inverse = projection_perspective_inverse(fov, aspect, z_min, z_max);
}

std::string open_shader(std::string const& shader_name)
{
	if (shader_name == "planet_frag")
	{
#include "../build/assets/shaders/planet/planet.frag.glsl"
		return s;
	}
	else if (shader_name == "planet_vert")
	{
#include "../build/assets/shaders/planet/planet.vert.glsl"
		return s;
	}
	else if (shader_name == "normal_frag")
	{
#include "../build/assets/shaders/normal/normal.frag.glsl"
		return s;
	}
	else if (shader_name == "normal_vert")
	{
#include "../build/assets/shaders/normal/normal.vert.glsl"
		return s;
	}

	error_vcl("Shader not found");
	return "Error";
}