#version 330 

uniform mat4 camera;
in mat4 model;

in vec3 vert;
in vec3 vertNormal;

void main() {
	vec3 disp = vert + normalize(vertNormal) * 0.005;
	gl_Position = camera * model * vec4(disp, 1);
}

