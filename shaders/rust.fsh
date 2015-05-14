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

//perlin noise
uniform sampler3D Noise;

// varying:
in vec3 vPosition;
in vec3 vNormal;

//noise texture
in vec3 MCposition;

// output color:
out vec4 oColor;



//roughness for Beckman and Fresnel
//Beckman only has one roughness parameter which is the slope of facets
//Gaussian has 2 paramters thus using the x component of the RoughnessParams
uniform vec3 RoughnessParams = vec3(0.5f, 0.4f, 0.5f);
float solidAngle = 1;
uniform vec3 copper = vec3(184.0f/255.0f, 115.0f/255.0f, 51.0f/255.0f);



vec3 rust()
{
	const float NoiseScale = 0.07;
	vec3 Color1 = vec3(183/255.0f, 65/255.0f, 14/255.0f); // (0.8, 0.7, 0.0)
	vec3 Color2 = vec3(90/255.0f, 98/255.0f, 102/255.0f); // (0.6, 0.1, 0.0)
	
	vec4 noisevec = texture(Noise, MCposition * NoiseScale);
	float intensity = abs(noisevec[0] - 0.25) +
						abs(noisevec[1] - 0.125) +
						abs(noisevec[2] - 0.0625) +
						abs(noisevec[3] - 0.03125);
	intensity = clamp(intensity * 10.0, 0.0, 1.0);
	vec3 color = mix(Color1, Color2, intensity);
	
	return color;
}

vec3 Cook_TorrancePS()
{
	vec3 Normal = normalize(vNormal);
	vec3 Veye = normalize(-vPosition);
	vec3 LightDirPix = normalize(uLightSource[ 0 ].position - vPosition);
	vec3 vHalf = normalize(LightDirPix + Veye);

	float roughness = RoughnessParams.y;
	
	//Beckman model
	float NdotH = max(0.0f, dot(Normal, vHalf));
	float normalDotHalf2 = NdotH * NdotH;
	float roughness2 = roughness * roughness;
	float exponent = -(1-normalDotHalf2) / (normalDotHalf2*roughness2);
	float e = 2.71828182845904523536028747135f;
	float D = pow(e,exponent) / (roughness2*normalDotHalf2*normalDotHalf2);


	//or mixture of Beckman model with the following parameters
	//m1 = 0.4
	//w1 = 0.4
	//m2 = 0.2
	//w2 = 0.6
	
	//geometric
	float NdotV = dot(Normal, Veye);
	float VdotH = dot(vHalf, Veye);
	float NdotL = dot(LightDirPix, Normal);
	float G1 = 2 * NdotH * NdotV / NdotH;
	float G2 = 2 * NdotH * NdotL / NdotH;
	float G = min(1.0f, max(0.0f, min(G1, G2)));
	
	//Fresnel term in original paper is approximated
	//SPECTRAL COMPOSITION OF THE REFLECTED LIGHT
	float R0 = RoughnessParams.z;
	float F = R0 + (1.0f - R0) * pow(1.0f - NdotL, 5.0);
	
	//Cook-Torrance
	//specular component
	float bidirectionalReflectanceSpecular = F * D * G / (NdotL * NdotV);
	float pi = 3.1415926535897932384626433832f;
	//attenuation by pi, this comes from the original paper
	//bidirectionalReflectance = s*Reflectance + d*Reflectance
	//this case s = 1, d = 0
	bidirectionalReflectanceSpecular /= pi;
	//Ireflect=R*Ilight*NdotL*solidAngle
	//Ilight = 1
	float CookTorranceSpecIntensity = bidirectionalReflectanceSpecular*NdotL*solidAngle;
	//from the original paper
	//The specular component represents light that is reflected from the surface of the material
	float Ispecular = max(0.0, CookTorranceSpecIntensity);

	
	
	//we will use Cook-Torrence reflectance for diffuse color channels
	//or a simple hack for the diffuse component as in the book
	//just use 0.3 of the diffuse intensity to make it lighter
	//we can ignore it completely if we want
	float Idiffuse = max(0.0f, NdotL)*0.3;
	
	
	
	//original paper
	//An impinging electromagnetic wave can stimulate
	//the motion of electrons near the surface, which in turn leads to reemission
	//(reflection) of a wave. reflection from a metal occurs essentially at the surface
	//The specular reflectance component has a copper color.
	//When the rms roughness slope m is small, multiple surface reflections may also be ignored
	//and the entire diffuse component disappears.
	//a correct treatment of the color of the specular component is needed
	//to obtain a realistic nonplastic appearance.


	vec3 Ctotal = vec3(0.0f, 0.0f, 0.0f);
	//do not want the NdotL to dominate, increase the CookTorrance intensity by pi
	//pi could be understood as the shininess factor and it could vary depending on each vertex
	
	//Ctotal += Idiffuse*copper + Ispecular * copper * pi;	
	
	//rusty
	vec3 rusty = rust();
	Ctotal += Idiffuse*rusty + Ispecular * rusty * pi;	

	return Ctotal;
}

void main() {
    vec3 color;

	color = Cook_TorrancePS();

    oColor = vec4(color,1.0);
}