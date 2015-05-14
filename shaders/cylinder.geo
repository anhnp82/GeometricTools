#version 330 compatibility
#extension GL_EXT_geometry_shader4 : enable

layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;

//eye space position
out vec3 gEyePosition;
out vec3 gNormal;
//model space position
out vec3 gModelPosition;
//aggregate 3 gMoveId from 3 input vertices
//identical for 3 vertices in a triagle => flat shading
out vec4 gMoveId;
//out vec4 gMoveId_adjacency;
//out vec4 gColor;

in vec3 vEyePosition[3];
in vec3 vNormal[3];
in vec3 vModelPosition[3];
in vec4 vColor[3];

uniform int ixSize = 256;
uniform int iySize = 256;
uniform float colorScale = 255;
uniform float size = 9;

void main() 
{
	///*
	//scale back
	vec4 scaledColor0 = vColor[0]*colorScale;
	vec4 scaledColor1 = vColor[1]*colorScale;
	vec4 scaledColor2 = vColor[2]*colorScale;

	//decode the moveIds from colors
	// We have to decrease by one 
	//because vertices without move id (vertices that were not cut currently) are set to color (0,0,0).
	gMoveId.x = scaledColor0.r * (iySize * ixSize) + scaledColor0.g * (ixSize) + scaledColor0.b - 1;
	gMoveId.y = scaledColor1.r * (iySize * ixSize) + scaledColor1.g * (ixSize) + scaledColor1.b - 1;
	gMoveId.z = scaledColor2.r * (iySize * ixSize) + scaledColor2.g * (ixSize) + scaledColor2.b - 1;
	
	//*/
	
	/*
	vec4 scaledColor0 = vColor[0]*size;
	vec4 scaledColor1 = vColor[1]*size;
	vec4 scaledColor2 = vColor[2]*size;
	gMoveId.x = scaledColor0.x;
	gMoveId.y = scaledColor1.x;
	gMoveId.z = scaledColor2.x;
	
	
	gMoveId_adjacency.x = scaledColor0.y;
	gMoveId_adjacency.y = scaledColor1.y;
	gMoveId_adjacency.z = scaledColor2.y;
	gMoveId_adjacency.w = 1;
	
	*/
	
	gMoveId.w = 1;
	
	//Pass-thru!
	for(int i=0; i< 3; i++)
	{
		gl_Position = gl_PositionIn[i];
		gEyePosition = vEyePosition[i];
		gNormal = vNormal[i];
		gModelPosition = vModelPosition[i];
		//if (round(gMoveId.x) == 1) EmitVertex();
		EmitVertex();
	}
}