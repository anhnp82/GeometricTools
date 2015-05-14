//---------------------------------------------------------------------------------
//	Phong shader and wood shading
//
//if we transform the vertex by a matrix M, we need to transform the normal by the matrix M inversed transposed
//Ctotal 	= Cambient + Cdiffuse + Cspecular;						//color components for each light source
//Cambient	= (Mambient * Lambient) + Memissive;					//ambient 
//Cdiffuse  = max(Normal.Light, 0) * Mdiffuse * Ldiffuse;			//diffuse
//Cspecular = pow(Reflect.Veye, Shininess) * Mspecular * Lspecular;	//specular
//Reflect	= 2(Normal.Light).Normal - Light;						//reflection vector
//the mul() intrinsic function takes order of operands into account
//---------------------------------------------------------------------------------

float4x4 matWorldViewProj;	//clipping space
float4x4 matWorld;			//world space
float4x4 matWorldInversedTransposed;
float4x4 matSpotLight;		//spot light range
float4 LightDir;			//sun light directions
bool switchSpot = true;		//turn on spot light and turn off sun light
float4 Veye;				//world space
float4 spotPosition;		//position of spot light

//float solidAngle = 0.0002;	//from original paper
float solidAngle = 1;

//model origin in world space
float4 modelOrigin = float4(124.000000f, 114.300003f, 60.000000f, 0.0f)/2;			

//light components
float4 Lambient 	= float4(1.0f, 1.0f, 1.0f, 1.0f);
float4 Lspecular 	= float4(1.0f, 1.0f, 1.0f, 1.0f);
float4 Ldiffuse 	= float4(1.0f, 1.0f, 1.0f, 1.0f);
//material components
//Mdiffuse taken from triangle list
//with ambient term we do not need to add another light source, just one is enough
float4 Mambient 	= float4(0.01f, 0.01f, 0.01f, 1.0f);
float4 Memissive 	= float4(0.005f, 0.005f, 0.005f, 1.0f);
float4 Mspecular	= float4(1.0f, 1.0f, 1.0f, 1.0f);
float Shininess		= 128.0f;	

float4 copper = {184.0f/255.0f, 115.0f/255.0f, 51.0f/255.0f, 1};

float4 darkWood = {133/255.0f, 94/255.0f, 66/255.0f, 1};

float4 lightWood = {238/255.0f, 197/255.0f, 145/255.0f, 1};//238;197;145

float4 rustColor = {183/255.0f, 65/255.0f, 14/255.0f, 1};

float4 ironGrey = {90/255.0f, 98/255.0f, 102/255.0f, 1};

float geometry_freq = 0.006;

float ring_freq
<
   string UIName = "ring_freq";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 0.003 );

//directions have 3 dims
//positions 4 dims
//colors 4 dims
struct VS_OUTPUT
{
	float4 Position			: POSITION;		//clipping space
	float4 Mdiffuse			: COLOR0;
	float3 Normal 			: TEXCOORD0;
	float3 Veye 			: TEXCOORD1;
	float3 Light1 			: TEXCOORD2;	// sun direction
	float3 Light2 			: TEXCOORD3;	// spotlight position
	float4 Pshade 			: TEXCOORD4;	//world position for wood shading
	float4 SpotPerspective	: TEXCOORD5; //perspective spotlight coordinates
};

VS_OUTPUT PhongVS(float4 Position : POSITION, float4 Normal : NORMAL, float4 Mdiffuse: COLOR0)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Position = mul(Position, matWorldViewProj); // transform into clipping space
	Out.Light1 = normalize(LightDir); 

	float4 PosWorld = mul(Position, matWorld);
	Out.Pshade = PosWorld;
	Out.Veye = normalize(Veye - PosWorld); 
	
	//spot light, in world space
	Out.Light2 = normalize(spotPosition - PosWorld);
	Out.SpotPerspective = mul(Position, matSpotLight);
	
	Out.Normal 	 = mul(Normal, matWorldInversedTransposed); 
	Out.Mdiffuse = Mdiffuse;
	return Out;
}

//texture for spotlight need to be initialize
Texture spotLightTexture;
sampler spotLightSampler = sampler_state 
{ 
	texture = <spotLightTexture> ; 
	magfilter = LINEAR; minfilter=LINEAR; 
	mipfilter = LINEAR; AddressU = clamp; 
	AddressV = clamp;
};

//pulse train sampler for wood shading
texture pulse_train_Tex;
texture volume_noise_Tex;

sampler pulse_train = sampler_state
{
   Texture = (pulse_train_Tex);
   ADDRESSU = WRAP;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};

sampler volumeNoise = sampler_state
{
   Texture = (volume_noise_Tex);
   ADDRESSU = WRAP;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};

//generate wood ring color based on world coordinate
/*
float4 woodRings(float4 Pshade0)
{
	float4 Pshade = Pshade0;
	if (!isMix) Pshade = Pshade0 - modelOrigin;
	float scaledDistFromZAxis = sqrt(dot(Pshade.xy, Pshade.xy)) * ring_freq;
	float blendFactor = tex1D (pulse_train, scaledDistFromZAxis);
	return lerp (darkWood, lightWood, blendFactor);
	//return darkWood * blendFactor + lightWood * (1 -blendFactor);
}
*/


texture Perlin_Tex;
sampler permTexture = sampler_state
{
	Texture = (Perlin_Tex);
	//ADDRESSU = WRAP;
	MAGFILTER = POINT;
	MINFILTER = POINT;
};

#define ONE 0.00390625
#define ONEHALF 0.001953125

float fade(float t) {
  //return t*t*(3.0-2.0*t); // Old fade
  return t*t*t*(t*(t*6.0-15.0)+10.0); // Improved fade
}
 
float noise(float3 P)
{
  float3 Pi = ONE*floor(P)+ONEHALF; 
                                 
  float3 Pf = P-floor(P);
  
  // Noise contributions from (x=0, y=0), z=0 and z=1
  float perm00 = tex2D(permTexture, Pi.xy).a ;
  float3  grad000 = tex2D(permTexture, float2(perm00, Pi.z)).rgb * 4.0 - 1.0;
  float n000 = dot(grad000, Pf);
  float3  grad001 = tex2D(permTexture, float2(perm00, Pi.z + ONE)).rgb * 4.0 - 1.0;
  float n001 = dot(grad001, Pf - float3(0.0, 0.0, 1.0));

  // Noise contributions from (x=0, y=1), z=0 and z=1
  float perm01 = tex2D(permTexture, Pi.xy + float2(0.0, ONE)).a ;
  float3  grad010 = tex2D(permTexture, float2(perm01, Pi.z)).rgb * 4.0 - 1.0;
  float n010 = dot(grad010, Pf - float3(0.0, 1.0, 0.0));
  float3  grad011 = tex2D(permTexture, float2(perm01, Pi.z + ONE)).rgb * 4.0 - 1.0;
  float n011 = dot(grad011, Pf - float3(0.0, 1.0, 1.0));

  // Noise contributions from (x=1, y=0), z=0 and z=1
  float perm10 = tex2D(permTexture, Pi.xy + float2(ONE, 0.0)).a ;
  float3  grad100 = tex2D(permTexture, float2(perm10, Pi.z)).rgb * 4.0 - 1.0;
  float n100 = dot(grad100, Pf - float3(1.0, 0.0, 0.0));
  float3  grad101 = tex2D(permTexture, float2(perm10, Pi.z + ONE)).rgb * 4.0 - 1.0;
  float n101 = dot(grad101, Pf - float3(1.0, 0.0, 1.0));

  // Noise contributions from (x=1, y=1), z=0 and z=1
  float perm11 = tex2D(permTexture, Pi.xy + float2(ONE, ONE)).a ;
  float3  grad110 = tex2D(permTexture, float2(perm11, Pi.z)).rgb * 4.0 - 1.0;
  float n110 = dot(grad110, Pf - float3(1.0, 1.0, 0.0));
  float3  grad111 = tex2D(permTexture, float2(perm11, Pi.z + ONE)).rgb * 4.0 - 1.0;
  float n111 = dot(grad111, Pf - float3(1.0, 1.0, 1.0));

  // Blend contributions along x
  float4 n_x = lerp(float4(n000, n001, n010, n011), float4(n100, n101, n110, n111), fade(Pf.x));

  // Blend contributions along y
  float2 n_xy = lerp(n_x.xy, n_x.zw, fade(Pf.y));

  // Blend contributions along z
  float n_xyz = lerp(n_xy.x, n_xy.y, fade(Pf.z));
 
  return n_xyz;
}

float turbulence(int octaves, float3 P, float lacunarity, float gain)
{	
  float sum = 0;
  float scale = 1;
  float totalgain = 1;
  for(int i=0;i<octaves;i++){
    sum += totalgain*noise(P*scale);
    scale *= lacunarity;
    totalgain *= gain;
  }
  return abs(sum);
}



//generate wood ring color based on world coordinate
float4 woodRingsPerlin(float4 Pshade)
{

  float x_v=Pshade.x*geometry_freq;
  float y_v=Pshade.y*geometry_freq;
  float z_v=Pshade.z*geometry_freq;
    
  float3 t1=float3(x_v*2,y_v*2,z_v*2);
  float3 t11=float3(x_v*2+1,y_v*2+1,z_v*2+1);
  float3 t2=float3(x_v*0,y_v*0,z_v*0);
    
  float temp_x=6000*noise(t1);
  float temp_y=6000*noise(t2);
  //float noisy = 6000*noise(t11);
  float noisy = 500*tex3D(volumeNoise, t1);
  
  x_v=x_v-temp_x+noisy;
  y_v=y_v-temp_y+noisy;

  float4 wood = lerp(darkWood, lightWood, frac((sqrt(x_v*x_v+y_v*y_v)/*+noisy*/)*ring_freq+30*turbulence(68, t2, 2, 0.6)));
  //float4 wood = lerp(darkWood, lightWood, frac(sqrt(x_v*x_v+y_v*y_v)*ring_freq+30*turbulence(68, t2, 2, 0.6)));
  //float4 wood = lerp(darkWood, lightWood, pow(frac(sqrt(x_v*x_v+y_v*y_v)+30*turbulence(68, t2, 2, 0.6)),3));
  //float4 wood = lerp(darkWood, lightWood, pow(cos(sqrt(x_v*x_v+y_v*y_v)+30*turbulence(68, t2, 2, 0.6))*0.5+0.5,3));
  return wood;
}

//generate rust color only on the boundary
//boundary condition based on implicit geometry representations
//it's actually granite
//http://www.rhino3d.com/5/help/Render/Procedural_Textures.htm
float4 rustPerlin(float4 Pshade)
{
	
  float4 rust = lerp(ironGrey, rustColor, clamp(0.7*turbulence(6,geometry_freq*Pshade.xyz,3,0.9), 0,1));
  //float4 rust = lerp(ironGrey, rustColor, cos(Pshade.z*0.1+6*turbulence(5,Pshade.xyz,2,0.6)));
  //float4 rust = lerp(ironGrey, rustColor, cos(Pshade.z*geometry_freq*0.1+6*turbulence(5,geometry_freq*Pshade.xyz,2,0.6)));
  return rust;
}

struct PS_INPUT
{
	float4 Mdiffuse		   : COLOR0;
	float3 Normal 		   : TEXCOORD0;
	float3 Veye 		   : TEXCOORD1;
	float3 Light1 		   : TEXCOORD2;
	float3 Light2 		   : TEXCOORD3;
	float4 Pshade 		   : TEXCOORD4;
	float4 SpotPerspective : TEXCOORD5; //perspective spotlight coordinates
};

float4 PhongPS(PS_INPUT input) : COLOR
{
	float3 Normal = normalize(input.Normal);
	float3 Veye = normalize(input.Veye);

	float3 LightDirPix;
	
	if (!switchSpot)
		LightDirPix = normalize(input.Light1);
	else
		LightDirPix = normalize(input.Light2);
	
	float4 Ctotal = {0.0f, 0.0f, 0.0f, 0.0f};

	//pixels with normals facing away from light source are not lit
	float Idiffuse = max(0.0f, dot(Normal, LightDirPix)); // diffuse component
	
	// R = 2 * (N.L) * N - L
	float3 Reflect = normalize(2 * Idiffuse * Normal - LightDirPix);
	float Ispecular = pow(max(0.0f, dot(Reflect, Veye)), Shininess); // R.V^n

	// I = Acolor + Dcolor * N.L + (R.V)n
	//mix the diffuse term with the wood color
	float4 Cwood = woodRingsPerlin(input.Pshade);
		
	Ctotal += Cwood * Idiffuse * Ldiffuse + Ispecular * Mspecular * Lspecular;
	
	//spot light attenuation
	if (switchSpot)
	{
		float4 SpotAttenuation = tex2Dproj(spotLightSampler, input.SpotPerspective);
		Ctotal *= SpotAttenuation;
	}

	//ambient must express material color as well
	//float4 Cambient = (input.Mdiffuse * Mambient) * Lambient + Memissive;
	//float4 Cambient = (lightWood * Mambient) * Lambient + Memissive;
	float4 Cambient = (lightWood * Mambient) * Lambient;
	
	return Cambient + Ctotal;	
}



struct VS_OUTPUT_CT {
	float4 Position			: POSITION; //Clipping space position
	float4 Mdiffuse			: COLOR0;
	float3 Light 			: TEXCOORD0; //
	float3 Veye 			: TEXCOORD1; //eye vector
	float3 vHalf 			: TEXCOORD2; //half vector
	float3 Normal			: TEXCOORD3;
	float4 SpotPerspective	: TEXCOORD4; //perspective spotlight coordinates
	float4 Pshade		    : TEXCOORD5;
};


struct PS_INPUT_CT
{
	float4 Mdiffuse		   : COLOR0;
	float3 Light 		   : TEXCOORD0;
	float3 Veye 		   : TEXCOORD1;
	float3 vHalf 		   : TEXCOORD2; //half vector
	float3 Normal 		   : TEXCOORD3;
	float4 SpotPerspective : TEXCOORD4; //perspective spotlight coordinates
	float4 Pshade		   : TEXCOORD5;
};


VS_OUTPUT_CT Cook_TorranceVS(float4 Position : POSITION, float4 Normal : NORMAL, float4 color : COLOR0)
{
	VS_OUTPUT_CT Out = (VS_OUTPUT_CT)0;
	Out.Position = mul(Position, matWorldViewProj); // transform into clipping space

	float4 PosWorld = mul(Position, matWorld);
	float3 toEye = normalize(Veye - PosWorld); 
	Out.Veye = toEye;
	
	//spot light, in world space
	float3 toLight = normalize(spotPosition - PosWorld);
	Out.Light = toLight;
	Out.SpotPerspective = mul(Position, matSpotLight);
	
	Out.Normal 	 = mul(Normal, matWorldInversedTransposed); 
	
	Out.vHalf = normalize(toLight + toEye);
	
	Out.Mdiffuse = color;
	
	Out.Pshade = PosWorld;
	
	return Out;
}


float4 Cook_TorrancePS(PS_INPUT_CT input) : COLOR
{
	float3 Normal = normalize(input.Normal);
	float3 Veye = normalize(input.Veye);
	float3 LightDirPix = normalize(input.Light);
	float3 vHalf = normalize(input.vHalf);

	//roughness for Beckman and Fresnel
	//Beckman only has one roughness parameter which is the slope of facets
	//Gaussian has 2 paramters thus using the x component of the RoughnessParams
	float3 RoughnessParams = {0.5f, 0.4f, 0.5f};
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

	float4 Ctotal = {0.0f, 0.0f, 0.0f, 0.0f};
	//do not want the NdotL to dominate, increase the CookTorrance intensity by pi
	//pi could be understood as the shininess factor and it could vary depending on each vertex
	//Ctotal += Idiffuse*copper + Ispecular * copper * pi;	
	
	//rusty
	float4 rusty = rustPerlin(input.Pshade);
	Ctotal += Idiffuse*rusty + Ispecular * rusty * pi;	
		
	//spot light attenuation
	if (switchSpot)
	{
		float4 SpotAttenuation = tex2Dproj(spotLightSampler, input.SpotPerspective);
		Ctotal *= SpotAttenuation;
	}

	//ambient must express material color as well
	//float4 Cambient = (copper * Mambient) * Lambient + Memissive;
	
	//rusty
	//float4 Cambient = (rusty * Mambient) * Lambient + Memissive;
	float4 Cambient = (rusty * Mambient) * Lambient;
	
	return Cambient + Ctotal;	
}



technique PhongWoodSpotLight
{
    pass P0
    {          
        VertexShader = compile vs_3_0 PhongVS(  );
        PixelShader  = compile ps_3_0 PhongPS(  ); 
    }
}


technique Cook_Torrance_Spot
{
    pass P0
    {          
        VertexShader = compile vs_3_0 Cook_TorranceVS(  );
        PixelShader  = compile ps_3_0 Cook_TorrancePS(  ); 
    }
}