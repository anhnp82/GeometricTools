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


vec3 sunSurface(vec3 diffuse)
{
	const float NoiseScale = 0.07;
	vec3 Color1 = vec3(0.8, 0.7, 0.0); // (0.8, 0.7, 0.0)
	vec3 Color2 = vec3(0.6, 0.1, 0.0); // (0.6, 0.1, 0.0)
	
	vec4 noisevec = texture(Noise, MCposition * NoiseScale);
	float intensity = abs(noisevec[0] - 0.25) +
						abs(noisevec[1] - 0.125) +
						abs(noisevec[2] - 0.0625) +
						abs(noisevec[3] - 0.03125);
	intensity = clamp(intensity * 6.0, 0.0, 1.0);
	vec3 color = mix(Color1, Color2, intensity);
	
	//vec3 color = vec3(intensity * diffuse);
	return color;
}


vec3 wood()
{
	const vec3 LightWoodColor = vec3(0.6, 0.3, 0.1);
	const vec3 DarkWoodColor = vec3(0.4, 0.2, 0.07);
	const float RingFreq = 0.2;		//for blending colors
	const float LightGrains = 1.0;
	const float DarkGrains = 0.0;
	const float GrainThreshold = 0.5;
	const vec3 NoiseScale = vec3(0.5, 0.1, 0.1)*0.07;	//noise sampling frequency
	const float Noisiness = 6.0;	//noise amlitude
	const float GrainScale = 0.8;	//light strips frequency
	
    vec3 noisevec = vec3(texture3D(Noise, MCposition * NoiseScale) *
                                                 Noisiness);
    vec3 location = MCposition + noisevec;

    float dist = sqrt(location.x * location.x + location.z * location.z);
    dist *= RingFreq;

    float r = fract(dist + noisevec[0] + noisevec[1] + noisevec[2]) * 2.0;

    if (r > 1.0)
        r = 2.0 - r;
 
    vec3 color = mix(LightWoodColor, DarkWoodColor, r);

    r = fract((MCposition.x + MCposition.z) * GrainScale + 0.5);
    noisevec[2] *= r;
    if (r < GrainThreshold)
        color += LightWoodColor * LightGrains * noisevec[2];
    else
        color -= LightWoodColor * DarkGrains * noisevec[2];
    
  return color;
}


vec3 diffuseTerm( in int lightNumber ) {

    vec3 c_l     = uLightSource[ lightNumber ].color;
    vec3 alpha_d = sunSurface(uDiffuseMaterial); // replace the diffuse color by a texture look up
	//vec3 alpha_d = wood();

    vec3 l       = uLightSource[ lightNumber ].position;
    vec3 p       = vPosition;

    vec3 n = normalize( vNormal );
	
	//n = vec3(0, 0, 1);

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
	//oColor = vec4(0, 0, 0, 1.0);
}
