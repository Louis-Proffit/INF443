std::string water_frag = R"(
#version 330 core

in struct fragment_data
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

layout(location=0) out vec4 FragColor;



uniform sampler2D image_texture;

uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;
uniform sampler2D dudvmap;
uniform sampler2D normalMap;

uniform vec3 light = vec3(1.0, 1.0, 1.0);

uniform vec3 color = vec3(0, 0.8, 0.8); // Unifor color of the object
uniform float alpha = 1.0f; // alpha coefficient
uniform float Ka = 0.4; // Ambient coefficient
uniform float Kd = 0.8; // Diffuse coefficient
uniform float Ks = 0.4f;// Specular coefficient
uniform float specular_exp = 64.0; // Specular exponent
uniform bool use_texture = true;
uniform bool texture_inverse_y = false;
uniform float movefactor;

const float waveStrength = 0.003;
const float shineDamper = 20.0;
const float reflectivity = 0.6;

uniform float fog_falloff = 0.00;
uniform bool use_fog = false;

void main()
{
	/*vec2 uv_image = vec2(fragment.uv.x, 1.0-fragment.uv.y);
	if(texture_inverse_y) {
		uv_image.y = 1.0-uv_image.y;
	}
	vec4 color_image_texture = texture(image_texture,uv_image);
	
	if(use_texture==false) {
		color_image_texture=vec4(1.0,1.0,1.0,1.0);
	}
	
	vec3 color_object  = vec3(0,0,0);
	vec3 color_shading = (Ka + Kd * diffuse) * color_object + Ks * specular * vec3(1.0, 1.0, 1.0);*/
	vec4 bla = texture(image_texture,fragment.uv);

	vec2 ndc = (fragment.clipSpace.xy/fragment.clipSpace.w)/2.0 +0.5;
	vec2 refractTexCoords = vec2(ndc.x,ndc.y);
	vec2 reflectTexCoords = vec2(ndc.x,-ndc.y);

	vec2 distortedTexCoords = texture(dudvmap, vec2(fragment.textureCoords.x + movefactor, fragment.textureCoords.y)).rg*0.1;
	distortedTexCoords = fragment.textureCoords + vec2(distortedTexCoords.x, distortedTexCoords.y+movefactor);
	vec2 total = (texture(dudvmap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

	reflectTexCoords+=total;
	refractTexCoords+=total;

	vec4 reflectColour = texture(reflection_texture,reflectTexCoords);
	vec4 refractColour = texture(refraction_texture,refractTexCoords);

	vec3 viewVector = normalize(fragment.toCameraVector);

	float refractiveFactor = dot(viewVector, vec3(0,0,1));

	vec4 normalMapColor = texture(normalMap, distortedTexCoords);
	vec3 normal = vec3(normalMapColor.r*2.0-1.0,normalMapColor.g*2.0-1.0, normalMapColor.b);
	normal = normalize(normal);

	/*if (gl_FrontFacing == false) {
		normal = -normal;
	}
	vec3 L = normalize(light-fragment.position);

	float diffuse = max(dot(normal,L),0.0);
	float specular = 0.0;
	if(diffuse>0.0){
		vec3 R = reflect(-L,normal);
		vec3 V = normalize(fragment.eye-fragment.position);
		specular = pow( max(dot(R,V),0.0), specular_exp );
	}*/

	vec3 reflectedLight = reflect(normalize(fragment.fromLightVector), normal);
	float specular2 = max(dot(reflectedLight, viewVector), 0.0);
	specular2 = pow(specular2, shineDamper);
	vec3 specularHighlights = vec3(1.0,1.0,1.0) * specular2 * reflectivity;

	FragColor = mix(reflectColour,refractColour,refractiveFactor);
	/*FragColor = vec4((Ka + Kd * diffuse) * mix(FragColor, vec4(0.0,0.3,0.5,1.0),0.2).xyz + Ks * specular * vec3(1.0, 1.0, 1.0), mix(FragColor, vec4(0.0,0.3,0.5,1.0),0.2).a);*/ 

	float depth = length(fragment.eye-fragment.position);
	float w_depth = exp(-fog_falloff*depth*depth);
	 

	FragColor = mix(FragColor, vec4(0.0,0.3,0.5,1.0),0.2)+ vec4(specularHighlights,0.0);
	if (use_fog) {
	vec3 color_with_fog = w_depth*FragColor.xyz+(1-w_depth)*vec3(0.7,0.7,0.7);

	FragColor = vec4(color_with_fog, FragColor.a);}
}
)";