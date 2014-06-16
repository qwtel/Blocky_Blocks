#version 330

uniform int UseCelShade;

uniform struct Light {
    vec3 position;
    vec3 intensities;
    float attenuation;
    float ambientCoefficient;
} light;

in vec3 fragVert;
in vec3 fragNormal;

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

float celShade(float shade) {
    if (shade < A) shade = 0.0;
    else if (shade < B) shade = B;
    else if (shade < C) shade = C;
    else if (shade < D) shade = D;
    else if (shade < E) shade = E;
    else if (shade < F) shade = F;
    else if (shade < G) shade = G;
    else shade = H;
    return shade;
}

void main() {
    mat4 model = fragModel;

    vec3 normal = normalize(transpose(inverse(mat3(model))) * fragNormal);
    vec3 surfacePos = vec3(model * vec4(fragVert, 1));
    vec4 surfaceColor = vec4((fragMaterialColor / 255.0), 1);
    vec3 surfaceToLight = normalize(light.position - surfacePos);

    //ambient
    vec3 ambient = light.ambientCoefficient * (surfaceColor.rgb * light.intensities);

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));

    if (UseCelShade == 1)
        diffuseCoefficient = celShade(diffuseCoefficient);

    vec3 diffuse = diffuseCoefficient * (surfaceColor.rgb * light.intensities);

    //attenuation
    float distanceToLight = length(light.position - surfacePos);
    float attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

    //linear color (color before gamma correction)
    vec3 linearColor = ambient + attenuation * diffuse;

    finalColor = vec4(linearColor, 0.5);
}