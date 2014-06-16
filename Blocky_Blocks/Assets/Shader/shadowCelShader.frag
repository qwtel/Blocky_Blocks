#version 330

uniform int UseCelShade;

uniform mat4 model;
uniform vec3 cameraPosition;

uniform sampler2D tex;
uniform sampler2D targetTex;
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

in vec3 fragVert;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec4 fragTargetTexCoord;

in vec4 shadowCoord;

out vec4 finalColor;

const float bias = 0.0005;

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
    
    float visibility = 1;
    
    if (shadowCoord.w > 0) {
        vec2 offset = shadowCoord.xy;
        offset.x -= 0.08;
        offset.y -= 0.08;

        visibility = texture(shadowMap, vec3((offset / shadowCoord.w)+bias, (shadowCoord.z - bias) / shadowCoord.w));

        for(int i=0;i<7;i++){
            offset.x += 0.02;
            for(int i=0;i<7;i++){
                offset.y += 0.02;
                visibility += texture(shadowMap, vec3((offset / shadowCoord.w)+bias, (shadowCoord.z - bias) / shadowCoord.w));
            }
        }
        
        visibility /= 64;
        visibility += 0.75; // shadows shouldn't be pitch-black
        visibility = min(visibility, 1);
    }

    vec3 normal = normalize(transpose(inverse(mat3(model))) * fragNormal);
    vec4 surfaceColor = vec4((material.color / 255.0), 1) * texture(tex, fragTexCoord);

    // target projection
    if (fragTargetTexCoord.w > 0) {
        vec2 ttc = vec2(fragTargetTexCoord.xy / fragTargetTexCoord.w);

        vec4 targetColor = vec4(1,1,1,1); 

        if (ttc.x >= 0 && ttc.y >= 0 && ttc.x <= 1 && ttc.y <= 1) {
            targetColor = texture(targetTex, ttc);
        }

        surfaceColor *= targetColor;
    }

    vec3 surfacePos = vec3(model * vec4(fragVert, 1));
    vec3 surfaceToLight = normalize(light.position - surfacePos);
    vec3 surfaceToCamera = normalize(cameraPosition - surfacePos);

    float cosDir = dot(surfaceToLight, -light.direction);
    float spotEffect = smoothstep(0.8,0.98, cosDir);

    if (UseCelShade == 1)
        spotEffect = celShade(spotEffect);
 
    float heightAttenuation = smoothstep(light.range, 0.0, length(surfaceToLight));

    //ambient
    vec3 ambient = light.ambientCoefficient * (surfaceColor.rgb * light.intensities);

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));

    if (UseCelShade == 1)
        diffuseCoefficient = celShade(diffuseCoefficient);

    vec3 diffuse = diffuseCoefficient * (surfaceColor.rgb * light.intensities);

    //specular
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0) {
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), material.shininess);
    }

    if (UseCelShade == 1)
        specularCoefficient = celShade(specularCoefficient);

    vec3 specular = specularCoefficient * material.specularColor * light.intensities;

    specular *= 0.5;

    //attenuation
    float distanceToLight = length(light.position - surfacePos);
    float attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

    //linear color (color before gamma correction)
    vec3 linearColor = ambient + attenuation * (diffuse + specular) * spotEffect * heightAttenuation;

    finalColor = visibility * vec4(linearColor, 1); /* * 0.0 + vec4(normalize(fragNormal), 1) / 0.5 + 0.5;*/
}