#version 330

uniform sampler2D tex;
uniform vec3 cameraPosition;

uniform struct Light {
	vec3 position;
	vec3 intensities;
	float attenuation;
	float ambientCoefficient;
} light;

uniform struct Material {
	vec3 color;
	vec3 specularColor;
	float shininess;
} material;

in vec3 fragVert;
in vec3 fragNormal;
in vec2 fragTexCoord;

flat in mat4 fragModel;
flat in vec3 fragMaterialColor;

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
	mat4 model = fragModel;

	vec3 normal = normalize(transpose(inverse(mat3(model))) * fragNormal);
	vec3 surfacePos = vec3(model * vec4(fragVert, 1));
	vec4 surfaceColor = vec4((fragMaterialColor / 255.0), 1) * texture(tex, fragTexCoord);
	vec3 surfaceToLight = normalize(light.position - surfacePos);
	vec3 surfaceToCamera = normalize(cameraPosition - surfacePos);

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
	vec3 linearColor = ambient + attenuation * (diffuse + specular);

	finalColor = vec4(linearColor, 1);
}