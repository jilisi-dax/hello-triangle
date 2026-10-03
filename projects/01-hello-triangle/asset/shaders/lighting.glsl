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
uniform float lightRange[MAX_LIGHTS];
uniform int shadowFlags[MAX_LIGHTS];

vec3 calcLighting(vec3 norm, vec3 FragPos, float shadow)
{
     // ambient环境光
    vec3 lighting = vec3(0.15f);
    for (int i = 0; i < lightCount; i++)
    {

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

        float shadowMask = (shadowFlags[i] == 1) ? shadow : 1.0;
        float rangeWindow = smoothstep(lightRange[i], lightRange[i] * 0.7, dist);
        lighting += attenuation * inCone * shadowMask * rangeWindow * (diffuse + specular);
        //lighting += attenuation * inCone * shadow * (diffuse + specular);
    }
    return lighting;
}
