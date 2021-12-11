std::string forest_vert = R"(
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
	vec3 eye;
} fragment;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 plane = vec4(0 ,0 , -1 ,1);

void main()
{
	fragment.position = vec3(model * vec4(position,1.0));
	fragment.normal   = vec3(model * vec4(normal  ,0.0));
	fragment.color = color;
	fragment.uv = uv;
	fragment.eye = vec3(inverse(view)*vec4(0,0,0,1.0));

	fragment.uv = uv/noise; 
	fragment.uv.x += parrallel.x/noise;
	fragment.uv.y += parrallel.y/noise;

	gl_ClipDistance[0] = dot(model*vec4(position,1.0), plane);
	gl_Position = projection * view * model * vec4(position, 1.0);
}
)";