#version 330

uniform mat4 model;
uniform vec3 cameraPosition;

uniform sampler2D tex;
uniform sampler2DShadow shadowMap;

uniform struct Light {
	vec3 position;
	vec3 intensities;
	float attenuation;
	float ambientCoefficient;
	vec3 direction;
	float range;
} light;

uniform struct Material {
	vec3 color;
	vec3 specularColor;
	float shininess;
} material;


vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

in vec3 fragVert;
in vec3 fragNormal;
in vec2 fragTexCoord;
//in vec3 position_worldspace;
//in vec3 normal_cameraspace;
//in vec3 eyeDirection_cameraspace;
//in vec3 lightDirection_cameraspace;
in vec4 shadowCoord;

out vec4 finalColor;

const float A = 0.1;
const float B = 0.15;
const float C = 0.3;
const float D = 0.45;
const float E = 0.6;
const float F = 0.75;
const float G = 0.9;
const float H = 1.0;

void main() {
	
	/*
	float visibility = 1;
	float bias = 0.005;

	if ( texture( shadowMap, (shadowCoord.xy / shadowCoord.w) ).z  < (shadowCoord.z-bias) / shadowCoord.w ) {
	    visibility = 0.5;
	}
	*/

	float bias = 0.0005;

	float visibility = 1;
	
	if (shadowCoord.w > 0) {
		visibility = texture(shadowMap, vec3(shadowCoord.xy / shadowCoord.w, (shadowCoord.z - bias) / shadowCoord.w));
		visibility += 0.75;
		visibility = min(visibility, 1);
	}

	/*
	for (int i=0;i<4;i++){

		int index = i;
		visibility -= 0.2*(1.0-texture( shadowMap, vec3(shadowCoord.xy + poissonDisk[index]/700.0,  (shadowCoord.z-bias)/shadowCoord.w) ));
	}
	*/

	vec3 normal = normalize(transpose(inverse(mat3(model))) * fragNormal);
	vec3 surfacePos = vec3(model * vec4(fragVert, 1));
	vec4 surfaceColor = vec4((material.color / 255.0), 1) * texture(tex, fragTexCoord);
	vec3 surfaceToLight = normalize(light.position - surfacePos);
	vec3 surfaceToCamera = normalize(cameraPosition - surfacePos);

	float cosDir = dot(surfaceToLight, -light.direction);
	float spotEffect = smoothstep(0.8,0.98, cosDir);
 
	float heightAttenuation = smoothstep(light.range, 0.0, length(surfaceToLight));
	//ambient
	vec3 ambient = light.ambientCoefficient * (surfaceColor.rgb * light.intensities);

	//diffuse
	float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));

	if (diffuseCoefficient < A) diffuseCoefficient = 0.0;
	else if (diffuseCoefficient < B) diffuseCoefficient = B;
	else if (diffuseCoefficient < C) diffuseCoefficient = C;
	else if (diffuseCoefficient < D) diffuseCoefficient = D;
	else if (diffuseCoefficient < E) diffuseCoefficient = E;
	else if (diffuseCoefficient < F) diffuseCoefficient = F;
	else if (diffuseCoefficient < G) diffuseCoefficient = G;
	else diffuseCoefficient = H;

	vec3 diffuse = diffuseCoefficient * (surfaceColor.rgb * light.intensities);

	//specular
	float specularCoefficient = 0.0;
	if(diffuseCoefficient > 0.0) {
	    specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), material.shininess);
	}

	if (specularCoefficient < A) specularCoefficient = 0.0;
	else if (specularCoefficient < B) specularCoefficient = B;
	else if (specularCoefficient < C) specularCoefficient = C;
	else specularCoefficient = D;

	specularCoefficient *= 0.5;

	vec3 specular = specularCoefficient * material.specularColor * light.intensities;

	//attenuation
	float distanceToLight = length(light.position - surfacePos);
	float attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

	//linear color (color before gamma correction)
	vec3 linearColor = ambient + attenuation* (diffuse + specular)*spotEffect*heightAttenuation;

	finalColor = visibility * vec4(linearColor, 1);
}