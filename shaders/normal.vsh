#version 330 compatibility

// for the lighting:
out vec3 vPosition;
out vec3 vNormal;

//model position
out vec3 modelPosition;	



void main() {

    vNormal = normalize(gl_NormalMatrix * gl_Normal);

    // eye space position:
    vPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	//model position
	modelPosition = vec3(gl_Vertex);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

