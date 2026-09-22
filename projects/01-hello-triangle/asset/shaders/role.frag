#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

#include "lighting.glsl"

uniform sampler2D ourTexture;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lighting = calcLighting(norm, FragPos);
    FragColor = texture(ourTexture, TexCoord) * vec4(lighting, 1.0);
}