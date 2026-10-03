#version 330 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D screenTex;

void main()
{
    vec2 texel = 1.0f / vec2(textureSize(screenTex, 0));
    float w[3] = float[](0.25, 0.5, 0.25);
    vec3 c = vec3(0.0);
    for (int x = -1; x <= 1; x++)
    for (int y = -1; y <= 1; y++)
        c += texture(screenTex, UV + vec2(x, y) * texel).rgb * w[x + 1] * w[y + 1];
    FragColor = vec4(c, 1.0);
}