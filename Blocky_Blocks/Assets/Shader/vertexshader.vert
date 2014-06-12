#version 330 

uniform mat4 camera;
uniform mat4 model;

in vec3 vert;

void main() {
	gl_Position = camera * model * vec4(vert, 1);
}


