
#version 330 core
layout (location = 0)in vec3 aPos;
layout (location = 1)in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;
uniform mat4 model;
uniform mat4 view; //相机观察矩阵
uniform mat4 projection;
uniform float time;

vec3 rodriguesRotate(vec3 v, vec3 k, float theta)
{
    float cosT = cos(theta);
    float sinT = sin(theta);
    return v * cosT + cross(k, v) * sinT + k * dot(k, v) * (1.0 - cosT);
}
void main()
{
	gl_Position = projection * view * model* vec4(aPos, 1.0);
	FragPos =  vec3(model * vec4(aPos, 1.0));
	Normal = aNormal;
    TexCoord = aTexCoord;
}