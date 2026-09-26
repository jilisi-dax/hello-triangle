#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;
out vec3 worldPos;
out vec4 lightSpacePos;
out vec3 Tangent;
out vec3 Bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos   = vec3(model * vec4(aPos, 1.0));
    worldPos  = FragPos;
    Normal    = normalMatrix * aNormal;
    TexCoord  = aTexCoord;

    vec3 n = normalize(Normal);
    lightSpacePos = lightSpaceMatrix * vec4(worldPos + n * 0.04, 1.0);
    
    vec3 T = normalize(normalMatrix * aTangent);
    T = normalize(T - n * dot(n, T));
    Tangent   = T;
    Bitangent = cross(n, T);
}