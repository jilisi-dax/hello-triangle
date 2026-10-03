#version 330 core
#include "common.glsl"

in vec2 UV;
out vec4 FragColor;

vec2 IntegrateBRDF(float NdotV, float roughness)
{
    vec3 V = vec3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);
    vec3 N = vec3(0.0, 0.0, 1.0);
    float A = 0.0, B = 0.0;
    for (uint i = 0u; i < 1024u; ++i)
    {
        vec2 Xi = Hammersley(i, 1024u);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);
        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);
        if (NdotL > 0.0)        //这下面没看懂，有空回来推一遍式子，
        {
            float k = (roughness * roughness) / 2.0;   // IBL 变体（直接光是 (r+1)²/8）
            float g1L = NdotL / (NdotL * (1.0 - k) + k);
            float g1V = NdotV / (NdotV * (1.0 - k) + k);
            float G_Vis = g1L * g1V * VdotH / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);
            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    return vec2(A, B) / 1024.0;
}

void main()
{
    FragColor = vec4(IntegrateBRDF(UV.x, UV.y), 0.0, 1.0);
}