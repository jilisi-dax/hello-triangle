#version 330 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D screenTex;
uniform int effectMode;   // 0=原样 1=灰度 2=反色 3=锐化

void main()
{
    vec4 color = texture(screenTex, UV);
    if (effectMode == 1)
    {
        float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
        color = vec4(vec3(gray), color.a);
    }
    else if (effectMode == 2)
    {
        color = vec4(vec3(1.0) - color.rgb, color.a);
    }
    else if (effectMode == 3)
    {
        const float kernel[9] = float[](
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
        );

        vec2 texel = 1.0f / vec2(textureSize(screenTex, 0));
        vec3 sum = vec3(0.0f);
        int i = 0;
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++)
                sum += kernel[i++] * texture(screenTex, UV + vec2(dx, dy) * texel).rgb;
        color = vec4(sum, color.a);
    }

    FragColor = color;
}