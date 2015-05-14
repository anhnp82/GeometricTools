#version 330 compatibility

// lighting:
uniform vec3 uAmbientLight;
uniform int numberOfLights = 1;

struct LightSourceParameters
{
    vec3 color;
    vec3 position;
};
uniform LightSourceParameters uLightSource[3];

// material:
uniform vec3  uAmbientMaterial;
uniform vec3  uDiffuseMaterial;
uniform vec3  uSpecularMaterial;
uniform float uShininessMaterial;



//uniform samplerRect sampler;
uniform sampler2D sampler;



uniform int textureSize = 600;


// varying:
in vec3 vPosition;
in vec3 vNormal;

//noise texture
in vec3 MCposition;

// output color:
out vec4 oColor;








void main() {
    vec3 color;
	

	vec4 tex = texelFetch(sampler, ivec2(814, 438), 0);

	//vec4 tex = texture(sampler, vec2(20, 20));
	
	//vec4 tex = textureRect(sampler, vec2(20, 20));
	
	//vec4 tex = texture(sampler, vec2(0.5, 0.5));

	oColor = vec4(tex.x, tex.y, tex.z, 1.0);
}
