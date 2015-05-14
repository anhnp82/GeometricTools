#version 330 compatibility
#extension GL_EXT_geometry_shader4 : enable

// for the lighting:
out vec3 vPosition;
out vec3 N;

//view or to eye V = -vPosition

void main() {

	N = normalize(gl_NormalMatrix * gl_Normal);

    // eye space position:
    vPosition = vec3(gl_ModelViewMatrix * gl_Vertex); 
	

	//half vector
	//H = normalize(L + V);
	//tangent vector


    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

