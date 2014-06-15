#version 330

uniform mat4 camera;
uniform mat4 model;
uniform mat4 depthBias;
uniform mat4 texGenMatrix;

in vec3 vert;
in vec3 vertNormal;
in vec2 vertTexCoord;

out vec3 fragVert;
out vec3 fragNormal;
out vec2 fragTexCoord;
out vec4 fragTargetTexCoord;
out vec4 shadowCoord;

void main() {
    fragTexCoord = vertTexCoord;
    fragNormal = vertNormal;
    fragVert = vert;

    gl_Position = camera * model * vec4(vert, 1);

    shadowCoord = depthBias * vec4(vert, 1);
    fragTargetTexCoord = texGenMatrix * vec4(vert, 1);
}