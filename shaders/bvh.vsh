#version 330 compatibility

// eye-space position
out vec3 gEyePosition;
out vec3 gNormal;
out vec3 gModelPosition;	
//out vec4 vColor;



void main() {

    gNormal = normalize(gl_NormalMatrix * gl_Normal);

    // eye space position:
    gEyePosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	//model position
	gModelPosition = vec3(gl_Vertex);
	
	//vColor = gl_Color;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

