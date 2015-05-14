#version 330 compatibility

// eye-space position
out vec3 vEyePosition;
out vec3 vNormal;
out vec3 vModelPosition;	
out vec4 vColor;

void main() {

    vNormal = normalize(gl_NormalMatrix * gl_Normal);

    // eye space position:
    vEyePosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	//model position
	vModelPosition = vec3(gl_Vertex);
	
	vColor = gl_Color;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

