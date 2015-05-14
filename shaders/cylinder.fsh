///////////////////////////////////////////////////////////////////////////
//bisection method taking into account overshooting problem only need 10 iterations
//this corresponds to the resolution of 1/2^10, equivalent to do 1000 brute search
///////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////

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

//perlin noise
//uniform sampler3D Noise;
//uniform samplerCube cubeMap;
uniform sampler2D begin_move;
uniform sampler2D end_move;
uniform sampler2D knn_move;
uniform sampler2D knn2_move;
uniform sampler2D knn3_move;
uniform sampler2D knn4_move;



uniform int global_tex_size = 1200;
uniform int NO_NEIGHBORS = 51;//51 for 16 neighbors per vertex

uniform vec3 orientation = vec3(0, 0, 1);
uniform float epsilon = 0.00001;
uniform float binaryIterations = 10.0f;

vec3 optimalPoint;

in vec3 gEyePosition;
in vec3 gNormal;
in vec3 gModelPosition;
in vec4 gMoveId;
//in vec4 gColor;

// output color:
out vec4 oColor;

//Largest float == 3.40282e+38
//Smallest float == 1.17549e-38
float maxFloat = 3.40282e+38;



//roughness for Beckman and Fresnel
//Beckman only has one roughness parameter which is the slope of facets
//Gaussian has 2 paramters thus using the x component of the RoughnessParams
uniform vec3 RoughnessParams = vec3(0.5f, 0.4f, 0.5f);
float solidAngle = 1;
uniform vec3 copper = vec3(184.0f/255.0f, 115.0f/255.0f, 51.0f/255.0f);//vec3(90/255.0f, 98/255.0f, 102/255.0f);



//ward parameters
vec3 R, T, N;
const float PI = 3.14159;
const float ONE_OVER_PI = 1.0 / PI;

uniform vec2 P = vec2(1, 2); // Diffuse (x) and specular reflectance (y)
uniform vec2 A = vec2 (0.1, 2); // Slope distribution in x and y
uniform vec3 Scale = vec3(1, 5, 1)*0.1; // Scale factors for intensity computation



vec3 closestPointToCylinder(vec3 baseCenter, float cylinderRadius, float cylinderHeight)
{
	vec3 closestPoint;
	
	vec3 endToPos = gModelPosition - baseCenter;
	float projection = dot(endToPos, orientation);
	
	//if (projection > cylinderHeight) return old_normal;//close to the arbor
	
	if (projection <= 0)//epsilon)//point to disk
	{
		vec3 pointOnPlane = gModelPosition - projection*orientation;
		vec3 toPointOnPlane = pointOnPlane - baseCenter;
		float toPointOnPlaneLength = length(toPointOnPlane);
		if ( toPointOnPlaneLength <= cylinderRadius ) closestPoint = pointOnPlane;// + 1*epsilon*orientation;
		else 
		{
			closestPoint = baseCenter + cylinderRadius * (toPointOnPlane/toPointOnPlaneLength);
		}
	}
	else //if (projection < cylinderHeight)
	{
		closestPoint = baseCenter + projection * orientation;
		closestPoint = closestPoint + normalize( gModelPosition - closestPoint )*(cylinderRadius);// - 10000*epsilon);
	}
	
	return closestPoint;
}



//if returned value <= 0 then no overshooting occurs
//update begin to the middle point
bool isBeginChanged(vec3 searchPoint, float cylinderRadius, float cylinderHeight, vec3 moveVec)
{
	vec3 closestPoint = closestPointToCylinder(searchPoint, cylinderRadius, cylinderHeight);
	//vec3 toClosestPoint = normalize(closestPoint - gModelPosition);
	vec3 toClosestPoint = (closestPoint - gModelPosition);
	float pointToBegin = dot(toClosestPoint, moveVec);
	return pointToBegin <= 0;
}



vec3 bisectionMethod(vec4 beginMoveTex, vec4 endMoveTex, vec3 moveVec, float moveLength, float projection)
{
	float cylinderHeight = beginMoveTex.w;
	float cylinderRadius = endMoveTex.w;
	vec3 beginMove = beginMoveTex.xyz;
	vec3 endMove = endMoveTex.xyz;
	
	vec3 searchPoint = endMove;
	vec3 closestPoint = endMove;
	
	for (float i = 0.0; i < binaryIterations; i++)
	{		
		searchPoint = (beginMove + endMove)/2.0f;
		bool beginChanged = isBeginChanged(searchPoint, cylinderRadius, cylinderHeight, moveVec);
		if (beginChanged) 
		{
			beginMove = searchPoint;
		}
		else 
		{
			endMove = searchPoint;
		}
	}
	
	optimalPoint = (beginMove+endMove)/2.0f;
	closestPoint = closestPointToCylinder(optimalPoint, cylinderRadius, cylinderHeight);
	
	return closestPoint;
}



vec3 computeNormal(vec3 old_normal)
{
	vec3 closestPoint;
	vec3 normal = vec3(0, 0, 1);
	float minLength = maxFloat;
	
	int moveIds[51];//51 for 16 neighbors
	moveIds[0] = int(round(gMoveId[0]));
	moveIds[1] = int(round(gMoveId[1]));
	moveIds[2] = int(round(gMoveId[2]));
	
	vec4 neighbors = texelFetch(knn_move, ivec2( mod(moveIds[0], global_tex_size), moveIds[0]/global_tex_size ), 0);
	moveIds[3] = int(round(neighbors.x));
	moveIds[4] = int(round(neighbors.y));
	moveIds[5] = int(round(neighbors.z));
	moveIds[6] = int(round(neighbors.w));
	
	neighbors = texelFetch(knn_move, ivec2( mod(moveIds[1], global_tex_size), moveIds[1]/global_tex_size ), 0);
	moveIds[7] = int(round(neighbors.x));
	moveIds[8] = int(round(neighbors.y));
	moveIds[9] = int(round(neighbors.z));
	moveIds[10] = int(round(neighbors.w));
	
	neighbors = texelFetch(knn_move, ivec2( mod(moveIds[2], global_tex_size), moveIds[2]/global_tex_size ), 0);
	moveIds[11] = int(round(neighbors.x));
	moveIds[12] = int(round(neighbors.y));
	moveIds[13] = int(round(neighbors.z));
	moveIds[14] = int(round(neighbors.w));
	
	neighbors = texelFetch(knn2_move, ivec2( mod(moveIds[0], global_tex_size), moveIds[0]/global_tex_size ), 0);
	moveIds[15] = int(round(neighbors.x));
	moveIds[16] = int(round(neighbors.y));
	moveIds[17] = int(round(neighbors.z));
	moveIds[18] = int(round(neighbors.w));
	
	neighbors = texelFetch(knn2_move, ivec2( mod(moveIds[1], global_tex_size), moveIds[1]/global_tex_size ), 0);
	moveIds[19] = int(round(neighbors.x));
	moveIds[20] = int(round(neighbors.y));
	moveIds[21] = int(round(neighbors.z));
	moveIds[22] = int(round(neighbors.w));
	
	neighbors = texelFetch(knn2_move, ivec2( mod(moveIds[2], global_tex_size), moveIds[2]/global_tex_size ), 0);
	moveIds[23] = int(round(neighbors.x));
	moveIds[24] = int(round(neighbors.y));
	moveIds[25] = int(round(neighbors.z));
	moveIds[26] = int(round(neighbors.w));
	
	neighbors = texelFetch(knn3_move, ivec2( mod(moveIds[0], global_tex_size), moveIds[0]/global_tex_size ), 0);
	moveIds[27] = int(round(neighbors.x));
	moveIds[28] = int(round(neighbors.y));
	moveIds[29] = int(round(neighbors.z));
	moveIds[30] = int(round(neighbors.w));
	
	neighbors = texelFetch(knn3_move, ivec2( mod(moveIds[1], global_tex_size), moveIds[1]/global_tex_size ), 0);
	moveIds[31] = int(round(neighbors.x));
	moveIds[32] = int(round(neighbors.y));
	moveIds[33] = int(round(neighbors.z));
	moveIds[34] = int(round(neighbors.w));
	
	neighbors = texelFetch(knn3_move, ivec2( mod(moveIds[2], global_tex_size), moveIds[2]/global_tex_size ), 0);
	moveIds[35] = int(round(neighbors.x));
	moveIds[36] = int(round(neighbors.y));
	moveIds[37] = int(round(neighbors.z));
	moveIds[38] = int(round(neighbors.w));
	
	/*
	moveIds[39] = 0; moveIds[40] = 0; moveIds[41] = 0; moveIds[42] = 0;
	moveIds[43] = 0; moveIds[44] = 0; moveIds[45] = 0; moveIds[46] = 0;
	moveIds[47] = 0; moveIds[48] = 0; moveIds[49] = 0; moveIds[50] = 0;
	*/
	
	///*
	
	neighbors = texelFetch(knn4_move, ivec2( mod(moveIds[0], global_tex_size), moveIds[0]/global_tex_size ), 0);
	moveIds[39] = int(round(neighbors.x));
	moveIds[40] = int(round(neighbors.y));
	moveIds[41] = int(round(neighbors.z));
	moveIds[42] = int(round(neighbors.w));
	
	neighbors = texelFetch(knn4_move, ivec2( mod(moveIds[1], global_tex_size), moveIds[1]/global_tex_size ), 0);
	moveIds[43] = int(round(neighbors.x));
	moveIds[44] = int(round(neighbors.y));
	moveIds[45] = int(round(neighbors.z));
	moveIds[46] = int(round(neighbors.w));
	
	neighbors = texelFetch(knn4_move, ivec2( mod(moveIds[2], global_tex_size), moveIds[2]/global_tex_size ), 0);
	moveIds[47] = int(round(neighbors.x));
	moveIds[48] = int(round(neighbors.y));
	moveIds[49] = int(round(neighbors.z));
	moveIds[50] = int(round(neighbors.w));
	
	//*/
	
	//int idOffset[3] = int[3](-1, 0, 1);
		
	for (int i = 0; i < NO_NEIGHBORS; i++)
	{
		//for (int j = 0; j < 3; j++)
		{
			//int idx = moveIds[i] + idOffset[j];
			int idx = moveIds[i];
		
			/*
			vec4 beginMoveTex = texelFetch(begin_move, ivec2( mod(idx, global_tex_size), idx/global_tex_size ), 0);
			vec4 endMoveTex = texelFetch(end_move, ivec2( mod(idx, global_tex_size), idx/global_tex_size ), 0);
			//*/
			
			///* torus cutter 20 diameter and 1 corner radius
			vec3 beginMove = texelFetch(begin_move, ivec2( mod(idx, global_tex_size), idx/global_tex_size ), 0).xyz;
			vec3 endMove = texelFetch(end_move, ivec2( mod(idx, global_tex_size), idx/global_tex_size ), 0).xyz;
			
			beginMove += orientation * 2;
			endMove += orientation * 2;
			vec4 beginMoveTex = vec4(beginMove, 48);
			vec4 endMoveTex = vec4(endMove, 13);
			//*/
			
			vec3 moveVec = endMoveTex.xyz - beginMoveTex.xyz;
			float moveLength = length(moveVec);
			moveVec /= moveLength;
			
			vec3 toPoint = gModelPosition - beginMove;
			float projection = dot(moveVec, toPoint);
			
			closestPoint = bisectionMethod(beginMoveTex, endMoveTex, moveVec, moveLength, projection);
			
			vec3 toClosestPoint = closestPoint - gModelPosition;
			float dist = dot(toClosestPoint, toClosestPoint);
			
			if(dist < minLength)
			{
				normal = toClosestPoint;
				minLength = dist;
				T = moveVec;
			}

		}
	}
		
	return gl_NormalMatrix * (normal);
}







/*
vec3 reflectionMap()
{
	vec3 BaseColor = vec3(184.0f/255.0f, 115.0f/255.0f, 51.0f/255.0f);//copper
	float MixRatio = 0.9;
	
	vec3 envColor = vec3(textureCube(cubeMap, normalize(R)));
	
	envColor = mix(envColor, BaseColor, MixRatio);
	return envColor;
}
*/

vec3 wardModel()
{
	float e1, e2, E, cosThetaI, cosThetaR, brdf, intensity;
	
	/*
	vec3 T = normalize( ( vPosition - dot(vPosition, N)*N ) );
	vec3 B = normalize( cross(T, N) );
	
	//*/
	
	//or T = beginMove - endMove
	
	/*
	vec3 featureDirection = vec3(0, 1, 0);
	vec3 T = normalize(cross(N, featureDirection));
	vec3 B = normalize(cross(N, T));
	//*/
	

	vec3 B = normalize(cross(N, T));
	vec3 L = normalize(uLightSource[ 0 ].position - gEyePosition);
	vec3 V = normalize(-gEyePosition);
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
	
	float Ispecular = max(0.0, intensity);
	float Idiffuse = max(0.0, cosThetaI);
	
	//vec3 color = (Ispecular + Idiffuse) * reflectionMap();
	vec3 color = (Ispecular + Idiffuse) * copper;
	
	return color;
}



vec3 Cook_TorrancePS()
{
	//vec3 Normal = normalize(gNormal);
	vec3 Normal = N;
	
	vec3 Veye = normalize(-gEyePosition);
	vec3 LightDirPix = normalize(uLightSource[ 0 ].position - gEyePosition);
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
	float Idiffuse = max(0.0f, NdotL)*0.0;
	
	
	
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
	Ctotal += Idiffuse*copper + Ispecular * copper * pi;	
	
	//rusty
	//vec3 rusty = rustPerlin(input.Pshade);
	//Ctotal += Idiffuse*rusty + Ispecular * rusty * pi;	


	//ambient must express material color as well
	//float4 Cambient = (copper * Mambient) * Lambient + Memissive;
	
	//rusty
	//float4 Cambient = (rusty * Mambient) * Lambient + Memissive;
	//float4 Cambient = (rusty * Mambient) * Lambient;
	
	//return Cambient + Ctotal;	
	return Ctotal;

}



void main() {
    vec3 color = vec3(0, 0, 0);
	T = vec3(0, 1, 0);
	
	N = normalize(gNormal);
	if (round(gMoveId.x) > -1 || round(gMoveId.y) > -1 || round(gMoveId.z) > -1) N = normalize(computeNormal(N));
	
	//compute reflection vector
	vec3 eyeDir;
	eyeDir = gEyePosition;
	R = normalize(reflect(eyeDir, N));
	
	color = 0.6*wardModel() + 0.4*Cook_TorrancePS();
	//color = 0.5*wardModel() + 0.5*Cook_TorrancePS();
	//color = wardModel();
	//color = Cook_TorrancePS();
	
	//color = N;

    oColor = vec4(color, 1.0);
}




