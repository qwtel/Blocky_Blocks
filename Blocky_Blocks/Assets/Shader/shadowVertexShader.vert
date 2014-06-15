#version 330

uniform mat4 camera;
uniform mat4 model;
//uniform mat4 view;
uniform mat4 depthBias;
//uniform vec3 lightInvDir;

in vec3 vert;
in vec3 vertNormal;
in vec2 vertTexCoord;

out vec3 fragVert;
out vec3 fragNormal;
out vec2 fragTexCoord;

//out vec3 normal_cameraspace;
//out vec3 eyeDirection_cameraspace;
//out vec3 lightDirection_cameraspace;
out vec4 shadowCoord;
//out vec3 position_worldspace;

void main() {
	fragTexCoord = vertTexCoord;
	fragNormal = vertNormal;
	fragVert = vert;

	gl_Position = camera * model * vec4(vert, 1);

	shadowCoord = depthBias * vec4(vert,1);
	
	//// Position of the vertex, in worldspace : M * position
	//position_worldspace = (model * vec4(vert,1)).xyz;
	
	//// Vector that goes from the vertex to the camera, in camera space.
	//// In camera space, the camera is at the origin (0,0,0).
	//eyeDirection_cameraspace = vec3(0,0,0) - ( view * model * vec4(vert,1)).xyz;

	//// Vector that goes from the vertex to the light, in camera space
	//lightDirection_cameraspace = (view*vec4(lightInvDir,0)).xyz;
	
	//// Normal of the the vertex, in camera space
	//normal_cameraspace = ( view * model * vec4(vertNormal,0)).xyz; // Only correct if ModelMatr
}