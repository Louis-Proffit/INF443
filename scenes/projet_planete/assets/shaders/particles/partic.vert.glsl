std::string partic_vert = R"(
#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs;	// Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; // Position of the center of the particule and size of the square

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 particlecolor;

// Values that stay constant for the whole mesh.
//uniform vec3 CameraRight_worldspace;
//uniform vec3 CameraUp_worldspace;
//uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraPosition;

uniform int nbRows;

vec3 CameraRight_worldspace= vec3(view[0][0], view[1][0], view[2][0]);
vec3 CameraUp_worldspace = vec3(view[0][1], view[1][1], view[2][1]);
mat4 VP = projection*view;

void main()
{
	float particleSize = xyzs.w; // because encoded this way.
	vec3 particleCenter_wordspace = xyzs.xyz;

	vec3 vertexPosition_worldspace =
		particleCenter_wordspace + CameraRight_worldspace * squareVertices.x * particleSize + CameraUp_worldspace * squareVertices.y * particleSize;

	// Output position of the vertex
	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

	// UV of the vertex. No special space for this one.
	UV = squareVertices.xy + vec2(0.5, 0.5);
	UV.y = 1 - UV.y;
	float row = float(nbRows);
	UV /= row;
	int number = int(color.z);
	UV.x += (number%nbRows)/row;
	UV.y += (number/nbRows)/row;
	//UV+=vec2(color.x,color.y);
	particlecolor = color;
}
)";