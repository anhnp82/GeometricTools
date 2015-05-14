#version 130	//OpenGL 3.0

// for the lighting:
out vec3 vPosition;
out vec3 vNormal;

//scale for noise texture
//64 texture size, 120 for bunny geometry so the scale factor is 64/120
uniform float Scale = 1;
//noise texture position
out vec3 MCposition;

void main() {
    vNormal = normalize(gl_NormalMatrix * gl_Normal);

    // eye space position:
    vPosition = vec3(gl_ModelViewMatrix * gl_Vertex); 
	
	//noise texture position
	MCposition = vec3(gl_Vertex) * Scale;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

