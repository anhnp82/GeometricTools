#version 130	//OpenGL 3.0

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

uniform samplerCube cubeMap;

// varying:
in vec3 vPosition;
in vec3 vNormal;

// output color:
out vec4 oColor;


vec3 reflectionMap()
{
	vec3 BaseColor = vec3(0.4, 0.4, 1.0);
	float MixRatio = 0.7;//0.8

	vec3 eyeDir = vPosition.xyz;//normalize(vPosition.xyz);
	vec3 n = normalize(vNormal);
	vec3 ReflectDir = reflect(eyeDir, n);//maybe transformed into world space
	
	vec3 envColor = vec3(textureCube(cubeMap, ReflectDir));
	
	envColor = mix(envColor, BaseColor, MixRatio);
	return envColor;
}





vec3 diffuseTerm( in int lightNumber ) {

    vec3 c_l     = uLightSource[ lightNumber ].color;
   
	vec3 alpha_d = reflectionMap();

    vec3 l       = uLightSource[ lightNumber ].position;
    vec3 p       = vPosition;

    vec3 n = normalize( vNormal );

    return max( vec3( (c_l*alpha_d) * ( dot(n, (l-p))/( length(l-p) )) ), vec3(0));
}

vec3 specularTerm( in int lightNumber ) {
    vec3 c_l     = uLightSource[ lightNumber ].color;
    vec3 alpha_sp= uSpecularMaterial;

    vec3 l       = uLightSource[ lightNumber ].position;
    vec3 p       = vPosition;

    vec3 n = normalize( vNormal );

    vec3 r = 2.0*n*dot(n,(l-p)) - (l-p);
    vec3 v = vec3(0);

    float rightSide = pow( ( dot(r, (v-p))/( length(r) * length(v-p) )), uShininessMaterial);

    return max( vec3( (c_l*alpha_sp) * rightSide ), vec3(0));
}

// ignored, that's why it's 1
float spot( in int lightNumber ) {
    return 1.0;
}

// ignored, that's why it's 1
float att( in int lightNumber ) {
    return 1.0;
}

void main() {
    vec3 color;

    vec3 ambientTerm = uAmbientLight * uAmbientMaterial;

    // all lights:
    color = ambientTerm;
    for (int i = 0; i < numberOfLights; i++) {
        color += spot(i) * att(i) * ( diffuseTerm(i) + specularTerm(i) );
    }

    oColor = vec4(color,1.0);
}
