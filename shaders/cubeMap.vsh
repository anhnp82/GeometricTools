#version 130	//OpenGL 3.0

// for the lighting:
out vec3 vPosition;
out vec3 vNormal;

void main() {

	vNormal = normalize(gl_NormalMatrix * gl_Normal);

    // eye space position:
    vPosition = vec3(gl_ModelViewMatrix * gl_Vertex); 

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

