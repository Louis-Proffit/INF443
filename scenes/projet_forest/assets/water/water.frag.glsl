std::string s = R"(
#version 330 core

in struct fragment_data
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
	vec3 eye;
	vec4 clipSpace;
} fragment;

layout(location=0) out vec4 FragColor;



uniform sampler2D image_texture;

uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;

uniform vec3 light = vec3(1.0, 1.0, 1.0);

uniform vec3 color = vec3(0, 0.8, 0.8); // Unifor color of the object
uniform float alpha = 1.0f; // alpha coefficient
uniform float Ka = 0.4; // Ambient coefficient
uniform float Kd = 0.8; // Diffuse coefficient
uniform float Ks = 0.4f;// Specular coefficient
uniform float specular_exp = 64.0; // Specular exponent
uniform bool use_texture = true;
uniform bool texture_inverse_y = false;


void main()
{
	vec3 N = normalize(fragment.normal);
	if (gl_FrontFacing == false) {
		N = -N;
	}
	vec3 L = normalize(light-fragment.position);

	float diffuse = max(dot(N,L),0.0);
	float specular = 0.0;
	if(diffuse>0.0){
		vec3 R = reflect(-L,N);
		vec3 V = normalize(fragment.eye-fragment.position);
		specular = pow( max(dot(R,V),0.0), specular_exp );
	}

	vec2 ndc = (fragment.clipSpace.xy/fragment.clipSpace.w)/2.0 +0.5;
	vec2 refractTexCoords = vec2(ndc.x,ndc.y);
	vec2 reflectTexCoords = vec2(ndc.x,-ndc.y);
	vec2 uv_image = vec2(fragment.uv.x, 1.0-fragment.uv.y);
	if(texture_inverse_y) {
		uv_image.y = 1.0-uv_image.y;
	}
	vec4 color_image_texture = texture(image_texture, uv_image);

	vec4 reflectColour = texture(reflection_texture,reflectTexCoords);
	vec4 refractColour = texture(refraction_texture,refractTexCoords);

	if(use_texture==false) {
		color_image_texture=vec4(1.0,1.0,1.0,1.0);
	}
	
	vec3 distortion1 = color_image_texture.rgb*2.0-1.0;
	vec3 color_object  = fragment.color * color*distortion1;
	vec3 color_shading = (Ka + Kd * diffuse) * color_object + Ks * specular * vec3(1.0, 1.0, 1.0);
	
	FragColor = mix(reflectColour,refractColour,0.5);
}
)";