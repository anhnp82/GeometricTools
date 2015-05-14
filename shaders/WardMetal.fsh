#version 330 compatibility
#extension GL_EXT_geometry_shader4 : enable

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
in vec3 vPosition, N;
vec3 R;

// output color:
out vec4 oColor;

const float PI = 3.14159;
const float ONE_OVER_PI = 1.0 / PI;

uniform vec2 P = vec2(0.2, 3); // Diffuse (x) and specular reflectance (y)
uniform vec2 A = vec2 (0.1, 5); // Slope distribution in x and y
uniform vec3 Scale = vec3(1, 5, 1)*0.2; // Scale factors for intensity computation

vec3 reflectionMap()
{
	vec3 BaseColor = vec3(184.0f/255.0f, 115.0f/255.0f, 51.0f/255.0f);//copper
	float MixRatio = 0.98;
	
	vec3 envColor = vec3(textureCube(cubeMap, normalize(R)));
	
	envColor = mix(envColor, BaseColor, MixRatio);
	return envColor;
}


vec3 wardModel()
{
	float e1, e2, E, cosThetaI, cosThetaR, brdf, intensity;
	
	/*
	vec3 T = normalize( ( vPosition - dot(vPosition, N)*N ) );
	vec3 B = normalize( cross(T, N) );
	
	//*/
	
	//or T = beginMove - endMove
	
	///*
	vec3 featureDirection = vec3(0, 1, 0);
	vec3 T = normalize(cross(N, featureDirection));
	vec3 B = normalize(cross(N, T));
	//*/
	
	vec3 L = normalize(uLightSource[ 0 ].position - vPosition);
	vec3 V = normalize(-vPosition);
	vec3 H = normalize(L + V);
	
	e1 = dot(H, T) / A.x;
	e2 = dot(H, B) / A.y;
	E = -2.0 * ((e1 * e1 + e2 * e2) / (1.0 + dot(H, N)));
	cosThetaI = dot(N, L);
	cosThetaR = dot(N, R);
	brdf = P.x * ONE_OVER_PI +
	P.y * (1.0 / sqrt(cosThetaI * cosThetaR)) *
	(1.0 / (4.0 * PI * A.x * A.y)) * exp(E);
	
	intensity = Scale[0] * P.x * ONE_OVER_PI +
	Scale[1] * P.y * cosThetaI * brdf +
	Scale[2] * dot(H, N) * P.y;
	
	vec3 color = intensity * reflectionMap();
	return color;
}






void main() {
    vec3 color;
	
	vec3 eyeDir;
	//compute reflection vector
	eyeDir = vPosition;
	R = normalize(reflect(eyeDir, N));
	
	color = wardModel();

    oColor = vec4(color, 1.0);
}
