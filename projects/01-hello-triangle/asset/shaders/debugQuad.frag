#version 330 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D depthMap;

void main()
{
    float d = texture(depthMap, UV).r;
    FragColor = vec4(vec3(d), 1.0);
}