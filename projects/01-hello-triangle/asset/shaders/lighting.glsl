// 共享光照块：需要灯光的片元着色器 #include 本文件
#define MAX_LIGHTS 8
uniform int lightCount;
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform vec3 spotDir[MAX_LIGHTS];
uniform float spotCutoff[MAX_LIGHTS];
uniform float spotCutoffOuter[MAX_LIGHTS];
uniform vec3 viewPos;
uniform float shininess;
uniform float specularStrength;
uniform sampler2D shadowMap;

vec3 calcLighting(vec3 norm, vec3 FragPos, float shadow)
{
    vec3 lighting = vec3(0.0);
    for (int i = 0; i < lightCount; i++)
    {
        // 环境光
        float ambientStrength = 0.2f;
        vec3 ambient = ambientStrength * lightColor[i];

        // 漫反射
        vec3 lightDir = normalize(lightPos[i] - FragPos);
        float diff = max(dot(norm, lightDir), 0.0f);
        vec3 diffuse = diff * lightColor[i];

        // 聚光锥（软边）
        vec3 lightToPix = normalize(FragPos - lightPos[i]);
        float theta = dot(lightToPix, normalize(spotDir[i]));
        float inCone = smoothstep(spotCutoffOuter[i], spotCutoff[i], theta);

        // 距离衰减
        float dist = length(lightPos[i] - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);

        // 高光
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfway = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfway, norm), 0.0f), shininess);
        vec3 specular = specularStrength * spec * lightColor[i];

        lighting += ambient + attenuation * inCone * shadow * (diffuse + specular);
    }
    return lighting;
}

float calcShadow(vec4 lightSpacePos, vec3 N, vec3 L)
{
    vec3 proj = lightSpacePos.xyz / lightSpacePos.w;
    proj = proj * 0.5 + 0.5;
    if (proj.z > 1.0) return 1.0;
    float bias = max(0.0005 * (1.0 - dot(N, L)), 0.00005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));  // 一个纹素多大
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float nearest = texture(shadowMap, proj.xy + vec2(x, y) * texelSize).r;
            shadow += (proj.z - bias > nearest) ? 0.0 : 1.0;
        }
    }
    return shadow / 9.0;
}