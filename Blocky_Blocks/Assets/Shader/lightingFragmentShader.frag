#version 330

uniform mat4 model;
uniform sampler2D tex;

uniform struct Light {
	vec3 position;
	vec3 intensities;
	float attenuation;
	float ambientCoefficient;
	vec3 cameraPosition;
} light;

uniform struct Material {
	vec3 color;
	vec3 specularColor;
	float shininess;
} material;

in vec3 fragVert;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 finalColor;

void main() {
	vec3 normal = normalize(transpose(inverse(mat3(model))) * fragNormal);
	vec3 surfacePos = vec3(model * vec4(fragVert, 1));
	vec4 surfaceColor = vec4((material.color / 255.0), 1) * texture(tex, fragTexCoord);
	vec3 surfaceToLight = normalize(light.position - surfacePos);
	vec3 surfaceToCamera = normalize(light.cameraPosition - surfacePos);

	//ambient
	vec3 ambient = light.ambientCoefficient * (surfaceColor.rgb * light.intensities);

	//diffuse
	float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
	vec3 diffuse = diffuseCoefficient * (surfaceColor.rgb * light.intensities);

	//specular
	float specularCoefficient = 0.0;
	if(diffuseCoefficient > 0.0) {
		specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), material.shininess);
	}
	vec3 specular = specularCoefficient * material.specularColor * light.intensities;

	//attenuation
	float distanceToLight = length(light.position - surfacePos);
	float attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

	//linear color (color before gamma correction)
    vec3 linearColor = ambient + attenuation * (diffuse + specular);

	finalColor = vec4(linearColor, 1);

	//final color (after gamma correction)
    //vec3 gamma = vec3(1.0/2.2);
    //finalColor = vec4(pow(linearColor, gamma), surfaceColor.a);
}