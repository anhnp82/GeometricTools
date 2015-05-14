#version 330 compatibility

// eye-space position
out vec3 vEyePosition;
out vec3 vNormal;
out vec3 vModelPosition;	
out vec4 vColor;
out vec3 vOrientation;

uniform float min_orientation = -1;
uniform float max_orientation = 1;

void main() {

    vNormal = normalize(gl_NormalMatrix * gl_Normal);

    // eye space position:
    vEyePosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	//model position
	vModelPosition = vec3(gl_Vertex);
	
	vColor = gl_Color;
	
	//vOrientation = gl_NormalMatrix * (vec3(gl_MultiTexCoord3) * (max_orientation - min_orientation) + min_orientation);

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

