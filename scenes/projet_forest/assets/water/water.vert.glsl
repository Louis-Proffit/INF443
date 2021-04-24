std::string s = R"(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 uv;

out struct fragment_data
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
	vec3 eye;
	vec4 clipSpace;
	vec2 textureCoords;
	vec3 toCameraVector;
	vec3 fromLightVector;
} fragment;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraPosition;

const float tiling = 1.0;

void main()
{
	fragment.position = vec3(model * vec4(position,1.0));
	fragment.normal   = vec3(model * vec4(normal  ,0.0));
	fragment.color = color;
	fragment.uv = uv;
	fragment.eye = vec3(inverse(view)*vec4(0,0,0,1.0));
	fragment.clipSpace =projection * view * model * vec4(position, 1.0);
	fragment.textureCoords = vec2(position.x/2.0+0.5,position.y/2.0+0.5)*tiling;
	fragment.toCameraVector = cameraPosition-fragment.position;
	fragment.fromLightVector = fragment.position - fragment.eye;


	gl_Position = fragment.clipSpace;
}
)";