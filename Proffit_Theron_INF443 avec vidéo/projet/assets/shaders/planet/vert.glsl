std::string planet_vert = R"(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 uv;
layout (location = 4) in float noise;
layout (location = 5) in vec3 parrallel;

out struct fragment_data
{
    	vec3 position;
    	vec3 normal;
    	vec3 color;
    	vec2 uv;
	float height_proportion;
	vec3 eye;
	
} fragment;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform float pulse_horizontal;
uniform float pulse_vertical;
uniform float height_horizontal;
uniform float height_vertical;

void main()
{
	float vertical_movement = pow(cos(time * pulse_horizontal + noise), 2);
	float horizontal_movement = pow(cos(time * pulse_vertical + noise), 2);
	vec3 new_position = (position + horizontal_movement * height_horizontal * parrallel) * (1 + height_vertical * vertical_movement);
	fragment.position = vec3(model * vec4(new_position,1.0));
	fragment.normal   = vec3(model * vec4(normal  ,0.0));
	fragment.color = color;
	fragment.uv = uv;
	fragment.height_proportion = vertical_movement;
	fragment.eye = vec3(inverse(view)*vec4(0,0,0,1.0));

	gl_Position = projection * view * model * vec4(new_position, 1.0);
}
)";