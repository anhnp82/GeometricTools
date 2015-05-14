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
//out vec4 gColor;

in vec3 vEyePosition[3];
in vec3 vNormal[3];
in vec3 vModelPosition[3];
in vec4 vColor[3];

uniform int ixSize = 256;
uniform int iySize = 256;
uniform float colorScale = 255;

void main() 
{
	//scale back
	vec4 scaledColor0 = vColor[0]*colorScale;
	vec4 scaledColor1 = vColor[1]*colorScale;
	vec4 scaledColor2 = vColor[2]*colorScale;

	//decode the moveIds from colors
	// We have to decrease by one 
	//because vertices without move id (vertices that were not cut currently) are set to color (0,0,0).
	gMoveId.x = round(scaledColor0.r * (iySize * ixSize) + scaledColor0.g * (ixSize) + scaledColor0.b - 1);
	gMoveId.y = round(scaledColor1.r * (iySize * ixSize) + scaledColor1.g * (ixSize) + scaledColor1.b - 1);
	gMoveId.z = round(scaledColor2.r * (iySize * ixSize) + scaledColor2.g * (ixSize) + scaledColor2.b - 1);

	gMoveId.w = 1;
	
	if (vModelPosition[0].z < 0 && vModelPosition[0].x < 35 && vModelPosition[0].x > 25 && vModelPosition[0].y < 20 && vModelPosition[0].y >10
	&& vModelPosition[1].z < 0 && vModelPosition[1].x < 35 && vModelPosition[1].x > 25 && vModelPosition[1].y < 20 && vModelPosition[1].y >10
	&& vModelPosition[2].z < 0 && vModelPosition[2].x < 35 && vModelPosition[2].x > 25 && vModelPosition[2].y < 20 && vModelPosition[2].y >10)
	{
	
		if (gMoveId.x == gMoveId.y && gMoveId.x == gMoveId.z)
		{
		
			//moveId = 19, 21
			if ( gMoveId.x == 19 )//|| gMoveId.x == 21 )
			{
	
				for(int i=0; i< 3; i++)
				{
				
					gl_Position = gl_PositionIn[i];
					gEyePosition = vEyePosition[i];
					gNormal = vNormal[i];
					gModelPosition = vModelPosition[i];

					EmitVertex();
				}
				
			}
		
		}
	
	}
}