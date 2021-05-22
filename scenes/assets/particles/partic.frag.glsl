std::string s = R"(
#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 particlecolor;

// Ouput data
out vec4 color;

uniform sampler2D myTexture;

void main(){
	vec4 flow = texture(myTexture,UV);
	if( flow.x < 0.3) {
		discard;
	}
	color = flow;

}
)";