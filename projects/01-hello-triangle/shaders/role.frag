
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

#define MAX_LIGHTS 8
uniform int lightCount;
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];

uniform float shininess;
uniform float specularStrength;
uniform vec3 viewPos;

uniform sampler2D ourTexture;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lighting = vec3(0.0);
    for (int i = 0; i < lightCount; i++)
    {
        // 环境光
        float ambientStrength = 0.2f;
        vec3 ambient = ambientStrength * lightColor[i];

        // 漫反射
        vec3 lightDir = normalize(lightPos[i] - FragPos);
        float diff = max(dot(norm, lightDir), 0.0f);
        vec3 diffuse = diff * lightColor[i];

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfway = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfway, norm), 0.0f), shininess);
        vec3 specular = specularStrength * spec * lightColor[i];

        lighting += ambient + diffuse + specular;
    }
	FragColor = texture(ourTexture, TexCoord)* vec4(lighting, 1.0);
}