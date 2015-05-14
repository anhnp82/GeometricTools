#version 330 compatibility

///////////////////////////////////////////////////////////////////////
////calculating in the model space for every pixel interpolated from the normal vertex shader
////after the normal has come out, transform it using normal matrix.
////for uniform grid, double resolution does not gain any performance benefit.
////to do: -fix the side effect, for vertical wall and the tool orientation is always upward
////		the tool head never touch the wall, only the tool body. 
////		Postpone to find a general solution for an arbitrary tool.
////	   -flip the normal in coarse regions where the normal pointing inward.
////	   
///////////////////////////////////////////////////////////////////////

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
uniform isampler2D hist_sampler;
uniform isampler2D associated_move_sampler;
uniform sampler2D begin_move;
uniform sampler2D end_move;


//radius of the tool head
uniform float radius = 1.5f;
//orientation of the tool
uniform vec3 orientation = vec3(0, 0, 1);

uniform float global_tex_size = 400;
uniform float hist_tex_size = 700;
uniform float list_tex_size = 1800;

uniform int noXCells = 150;
uniform int noYCells = 100;
uniform int noZCells = 30;

uniform vec3 bboxMax;
uniform vec3 bboxMin;

// varying:
in vec3 vPosition;
in vec3 vNormal;

//model position
in vec3 modelPosition;

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



float getCellId()
{

	//x=width, y=height, z=depth
	//z*y*x: inner most is listed first

	float xId, yId, zId;
	
	xId = floor(noXCells*(modelPosition.x - bboxMin.x)/(bboxMax.x - bboxMin.x));
	yId = floor(noYCells*(modelPosition.y - bboxMin.y)/(bboxMax.y - bboxMin.y));
	zId = floor(noZCells*(modelPosition.z - bboxMin.z)/(bboxMax.z - bboxMin.z));
	
	//ordering is important here
	float result = (zId*noYCells*noXCells + yId*noXCells + xId);
	
	return result;
}



vec3 computeNormal()
{
	vec3 closestPoint;
	vec3 normal = vec3(0, 0, 1);
	float minLength = maxFloat;
	
	
	
	float cellId = getCellId();
	ivec2 tex_coord = ivec2( int(mod(cellId, hist_tex_size)), int(floor(cellId/hist_tex_size)) );
	ivec4 list_info = texelFetch ( hist_sampler, tex_coord, 0 );
	
	int begin_width = list_info.r;
	int begin_height = list_info.g;
	int begin_channel = list_info.b;
	int list_length = list_info.a;
	

	
	/*
	if (begin_channel == 3)
	{
		normal = vec3(1, 0, 0);
	}
	return normal;
	//*/
	
	//compute everything in model coordinate. then use normal matrix to do the rest
	
	//int end_channel = int(mod(begin_channel + list_length - 1), 4);
	float absolute_length = (begin_height*list_tex_size + begin_width)*4 + begin_channel + list_length - 1;
	//int end_channel = int(mod(absolute_length, 4));
	float sequential_tex = floor(absolute_length/4);
	//int end_height = int(floor(sequential_tex/list_tex_size));
	//int end_width = int(mod(sequential_tex, list_tex_size));
	
	
	
	float increment = begin_height*list_tex_size + begin_width;
	//include redundant channels for easier processing
	while(increment <= sequential_tex)
	{
		int current_height = int(floor(increment/list_tex_size));
		int current_width = int(mod(increment, list_tex_size));
		
		ivec4 element = texelFetch ( associated_move_sampler, ivec2(current_width, current_height), 0 );
		for (int i = 0; i < 4; i++)
		{
			int idx = element[i];
			
			
			//idx is correct
			/*
			if (idx > 60000)
			{
				normal = vec3(1, 0, 0);
			}
			return normal;
			//*/
			
			//until now everything is correct
			
			
			vec3 beginMove = texelFetch(begin_move
									, ivec2( int(mod(idx, global_tex_size)), int(floor(idx/global_tex_size)) ), 0).rgb;
			vec3 endMove = texelFetch(end_move, ivec2( int(mod(idx, global_tex_size))
									, int(floor(idx/global_tex_size)) ), 0).rgb;
			
			
			
			///*
			float epsilon = abs(modelPosition.z - endMove.z);
			if (epsilon > 0.05) continue;
			//*/
			

			
			// Move toolpath to center of tool
			beginMove += orientation * radius;
			endMove += orientation * radius;

			// Calculate normal
			vec3 moveVec = endMove - beginMove;
			vec3 beginToPos = modelPosition - beginMove;

			float projection;
			float moveLength = sqrt(dot(moveVec, moveVec));
			if( moveLength == 0.0f )
			{
				projection = 0.0f;
			}
			else
			{
				moveVec /= (moveLength);
				projection = (dot(beginToPos, moveVec));
			}

			if (projection < 0)
			{
				closestPoint = beginMove;
			}
			else if ( projection > moveLength)
			{
				closestPoint = endMove;
			}
			else
			{
				closestPoint = beginMove + projection * moveVec;
			}

			vec3 dist = closestPoint - modelPosition;
			float squaredDist = dot(dist, dist);
			
			if(squaredDist < minLength)
			{
				normal = dist;
				minLength = squaredDist;
			}
			
		}
		
		increment++;
	}
		
	return gl_NormalMatrix * normal;
}



vec3 Cook_TorrancePS()
{
	vec3 Normal = normalize(vNormal);
	
	if (dot( Normal, (gl_NormalMatrix * orientation) ) > 0.00001) Normal = normalize(computeNormal());
	
	//return Normal;
	
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
    vec3 color;

	color = Cook_TorrancePS();

    oColor = vec4(color,1.0);
}




