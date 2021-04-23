std::string s = R"(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 uv;
layout (location = 4) in float noise;

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
uniform float pulse;
uniform float height;

void main()
{
	float height_proportion = pow(cos(time * pulse + noise), 2);
	vec3 new_position = position * (1 + height * height_proportion);
	fragment.position = vec3(model * vec4(new_position,1.0));
	fragment.normal   = vec3(model * vec4(normal  ,0.0));
	fragment.color = color;
	fragment.uv = uv;
	fragment.height_proportion = height_proportion;
	fragment.eye = vec3(inverse(view)*vec4(0,0,0,1.0));

	gl_Position = projection * view * model * vec4(new_position, 1.0);
}
)";