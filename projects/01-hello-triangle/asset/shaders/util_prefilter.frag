#version 330 core
#include "common.glsl"

out vec4 FragColor;

in vec3 dir;


uniform samplerCube environment;
uniform float roughness;

void main()
{

    vec3 N = normalize(dir);
    vec3 V = N;

    if (roughness < 0.03)
    {  // roughness→0 时 直接抄源图 mip0 跳过重要性采样（窄分布下点射会留规律噪点）
        FragColor = vec4(textureLod(environment, N, 0.0).rgb, 1.0);
        return;
    }

    const uint SAMPLE_COUNT = 1024u;
    const float resolution = 1024.0;   // 源天空盒每面分辨率
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;

    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);   // H 是 halfway：L 由 V 和 H 反推

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            float D = DistributionGGX(N, H, roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);//贴图立体角
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);//样本立体角
            float mip = 0.5 * log2(saSample / saTexel);

            prefilteredColor += textureLod(environment, L, mip).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;
    FragColor = vec4(prefilteredColor, 1.0);

}
