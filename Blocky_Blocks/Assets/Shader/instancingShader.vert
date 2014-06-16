#version 330

uniform mat4 camera;
in mat4 model;
in vec3 materialColor;

in vec3 vert;
in vec3 vertNormal;
in vec2 vertTexCoord;

out vec3 fragVert;
out vec3 fragNormal;
flat out mat4 fragModel;
flat out vec3 fragMaterialColor;

void main() {
	fragNormal = vertNormal;
	fragVert = vert;
	fragModel = model;
	fragMaterialColor = materialColor;

	gl_Position = camera * model * vec4(vert, 1);
}