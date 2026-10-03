#version 330 core
in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 worldPos;
in vec4 lightSpacePos;
in vec3 Tangent;
in vec3 Bitangent;

out vec4 FragColor;

#include "common.glsl"
#include "lighting.glsl"

uniform sampler2D ourTexture;
uniform sampler2D normalMap;
uniform vec3 materialColor;
uniform vec3 shadowLightDir;
uniform float cellSize = 2.0f;
uniform int albedoMode;   // 0=纹理×materialColor  1=程序化checker
uniform int normalMapEnabled;

void main()
{
    vec3 norm;
    if (normalMapEnabled == 1)
    {
        vec3 n = texture(normalMap, TexCoord).xyz * 2.0f - 1.0f;
        mat3 TBN = mat3(normalize(Tangent), normalize(Bitangent), normalize(Normal));
        norm = normalize(TBN * n);
    }
    else
    {
        norm = normalize(Normal);
    }
    vec3 albedo;
    if (albedoMode == 1)
    {
        vec2 cell = floor(worldPos.xz / cellSize);
        float checker = mod(cell.x + cell.y, 2.0);
        albedo = checker > 0.5 ? vec3(0.85f, 0.82f, 0.78f) : vec3(0.55f, 0.52f, 0.48f);
    }
    else
    {
        vec3 texColor = pow(texture(ourTexture, TexCoord).rgb, vec3(2.2));   // sRGB -> 线性
        albedo = texColor * materialColor;
    }

    float shadow = calcShadow(shadowMap, lightSpacePos, norm, normalize(-shadowLightDir));
    vec3 lighting = calcLighting(norm, FragPos, shadow);
    FragColor = vec4(albedo, 1.0) * vec4(lighting, 1.0);
}