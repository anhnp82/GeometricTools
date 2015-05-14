#version 330 compatibility

layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;



void main() 
{

	///*

	//Pass-thru!
	for(i=0; i< gl_VerticesIn; i++){
		gl_Position = gl_PositionIn[i];

		EmitVertex();
	}
	
	//*/
}