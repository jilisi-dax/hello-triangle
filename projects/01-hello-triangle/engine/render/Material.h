#pragma once
#include "core/pch.h"
#include "render/Shader.h"

class Texture2D;

class Material
{
public:
	Shader* m_shader = nullptr;
	Texture2D* diffuseMap = 0;
	float cellSize = 2.0f;
	glm::vec3 color = glm::vec3(1.0f);// 染色
	float shininess = 32.0f;// 高光锐度
	float specularStrength = 0.5f;
	Texture2D* normalMap = nullptr;
	int albedoMode = 0;   // 表皮模式：0=纹理 1=checker（将来映射 shader 变体）
	float metallic = 0.0f;
	float roughness = 0.5f;
	Texture2D* metallicMap = nullptr;
	Texture2D* roughnessMap = nullptr;
	Texture2D* aoMap = nullptr; //Occlusion / Roughness / Metallic
	Texture2D* ormMap = nullptr;

	~Material() {}

	void bind();

//private:
	static Material* CreateFromJson(const char* matPath);
};