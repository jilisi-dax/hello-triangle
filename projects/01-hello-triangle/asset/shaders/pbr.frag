#version 330 core
#include "common.glsl"

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 lightSpacePos;

out vec4 Fragcolor;

#define MAX_LIGHTS 8
uniform int lightCount;
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform vec3 viewPos;

uniform sampler2D ourTexture;
uniform vec3 materialColor;
uniform float metallic;
uniform float roughness;
uniform int albedoMode;

uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform int metallicMapEnabled;
uniform int roughnessMapEnabled;
uniform sampler2D normalMap;
uniform int normalMapEnabled;
uniform sampler2D aoMap;
uniform int aoMapEnabled;
uniform sampler2D ormMap;
uniform int ormMapEnabled;

uniform samplerCube irradianceMap;
uniform int irradianceEnabled;
uniform samplerCube prefilterMap;
uniform int prefilteredEnabled;
uniform sampler2D brdfLut;

uniform sampler2D shadowMap;
uniform vec3 shadowLightDir;
uniform vec3 spotDir[MAX_LIGHTS];
uniform float spotCutoff[MAX_LIGHTS];
uniform float spotCutoffOuter[MAX_LIGHTS];
uniform float lightRange[MAX_LIGHTS];
uniform int shadowFlags[MAX_LIGHTS];

void main()
{
        vec3 N;
    if (normalMapEnabled == 1)
    {
        vec3 n = texture(normalMap, TexCoord).xyz * 2.0f - 1.0f;
        mat3 TBN = mat3(normalize(Tangent), normalize(Bitangent), normalize(Normal));
        N = normalize(TBN * n);
    }
    else
    {
        N = normalize(Normal);
    }

    vec3 V = normalize(viewPos - FragPos);
    
    vec3 albedo;
    if (albedoMode == 2 )
        albedo = pow(materialColor, vec3(2.2));
    else
        albedo = pow(texture(ourTexture, TexCoord).rgb, vec3(2.2)) * materialColor;

    float metal = metallic;
    float rough = roughness;
    if (metallicMapEnabled == 1) metal = texture(metallicMap, TexCoord).r;
    if (roughnessMapEnabled == 1) rough = texture(roughnessMap, TexCoord).r;
    float ao = 1.0;
    if (aoMapEnabled == 1) ao = texture(aoMap, TexCoord).r;
    if (ormMapEnabled == 1)
    {
        vec3 orm = texture(ormMap, TexCoord).rgb;
        ao = orm.r; rough = orm.g; metal = orm.b;
    }
    //rough = max(rough, 0.045);   // 下限：防窄瓣欠采样闪烁
    vec3 Ngeo = normalize(N);
    float dN = length(fwidth(Ngeo));  // 每像素法线变化量
    rough = max(rough, sqrt(clamp(dN * 1.5625, 0.0, 0.25))); //加个动态下限，防止窄瓣欠采样闪烁(泛光圈小于单个像素的时候就会出现)

    vec3 F0 = mix(vec3(0.04), albedo, metal);

    vec3 Lo = vec3(0.0);

    vec3 Fenv  = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kDenv = (vec3(1.0) - Fenv) * (1.0 - metal);
    vec3 irradiance = (irradianceEnabled == 1) ? texture(irradianceMap, N).rgb : vec3(0.03);
    vec3 R = reflect(-V, N);
    vec3 prefilteredColor = (prefilteredEnabled == 1) ? textureLod(prefilterMap, R, rough * 4.0).rgb : vec3(0.03);
    //漫反射系数 * 反照率 * 辐照度 + 预滤波环境贴图 * BRDF的反射系数
    //vec3 ambient = kDenv * albedo * irradiance + prefilteredColor * Fenv;
    vec2 envBRDF = texture(brdfLut, vec2(max(dot(N, V), 0.0), rough)).rg;
    vec3 ambient = kDenv * albedo * irradiance + prefilteredColor * (F0 * envBRDF.x + envBRDF.y);
    ambient = ambient * ao;

    float shadow = calcShadow(shadowMap, lightSpacePos, N, normalize(-shadowLightDir));
    for (int i = 0; i < lightCount; ++i)
    {
        vec3 L = normalize(lightPos[i] - FragPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPos[i] - FragPos);
        vec3 radiance = lightColor[i] / (distance * distance);

        vec3 lightToPix = normalize(FragPos - lightPos[i]);
        float theta = dot(lightToPix, normalize(spotDir[i]));
        float inCone = smoothstep(spotCutoffOuter[i], spotCutoff[i], theta);
        float shadowMask = (shadowFlags[i] == 1) ? shadow : 1.0;
        float rangeWindow = smoothstep(lightRange[i], lightRange[i] * 0.7, distance);

        float NdotV = max(dot(N, V), 0.0);
        float NdotL = max(dot(N, L), 0.0);
        float D = DistributionGGX(N, H, rough);
        float G = GeometrySmith(N, V, L, rough);
        vec3  F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        float kSchlick = (rough + 1.0) * (rough + 1.0) / 8.0;
        float visNV = 1.0 / (NdotV * (1.0 - kSchlick) + kSchlick);
        float visNL = 1.0 / (NdotL * (1.0 - kSchlick) + kSchlick);
        vec3 specular = D * F * 0.25 * visNV * visNL;

        vec3 kD = (vec3(1.0) - F) * (1.0 - metal);
        //Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * inCone * shadowMask * rangeWindow;
    }
    vec3 color = ambient + Lo;
    color = min(color, vec3(100.0));
    Fragcolor = vec4(color, 1.0);


}
