#pragma once
#include "pch.h"
#include "Shader.h"

class Material
{
public:
	Shader* m_shader = nullptr;
	unsigned int diffuseMap = 0;
	float cellSize = 2.0f;
	glm::vec3 color = glm::vec3(1.0f);// 染色
	float shininess = 32.0f;// 高光锐度
	float specularStrength = 0.5f;
	unsigned int normalMap = 0;
	int albedoMode = 0;   // 表皮模式：0=纹理 1=checker（将来映射 shader 变体）
	float metallic = 0.0f;
	float roughness = 0.5f;
	unsigned int metallicMap = 0;
	unsigned int roughnessMap = 0;
	unsigned int aoMap = 0; //Occlusion / Roughness / Metallic
	unsigned int ormMap = 0;

	~Material() {}

	void bind();

//private:
	static Material* CreateFromJson(const char* matPath);
	static unsigned int LoadTexture(const char* path);
};