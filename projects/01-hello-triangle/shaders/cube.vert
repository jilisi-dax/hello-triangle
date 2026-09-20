
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
	float angle = time * 0.8;
	vec3 axis = vec3(1.0, -1.0, 0.0);
    axis = normalize(axis);

    vec3 rotatedPos = rodriguesRotate(aPos, axis, angle);
	vec3 rotatedNormal = rodriguesRotate(aNormal, axis, angle); // 法线也要跟着转！

	gl_Position = projection * view * vec4(rotatedPos, 1.0);
	FragPos =  rotatedPos;
	Normal = rotatedNormal;
    TexCoord = aTexCoord;
}