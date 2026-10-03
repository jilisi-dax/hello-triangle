#version 330 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D screenTex;
uniform int horizontal; // 1=水平 0=垂直

vec3 safeSample(sampler2D tex, vec2 uv)
{
    vec3 c = texture(tex, uv).rgb;
    return clamp(c, vec3(0.0), vec3(30.0));
}

void main()
{
    vec2 texel = 1.0f / vec2(textureSize(screenTex, 0));
    vec2 dir = horizontal == 1 ? vec2(texel.x, 0.0f) : vec2(0.0f, texel.y);
    
    float weights[5] = float[](
        0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f
    );
    vec3 sum = safeSample(screenTex, UV) * weights[0];

    for (int i = 1; i < 5; i++)
    {
        sum += safeSample(screenTex, UV + dir * float(i)) * weights[i];
        sum += safeSample(screenTex, UV - dir * float(i)) * weights[i];
    }
    FragColor = vec4(sum, 1.0);
}