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

	~Material() {}

	void bind();

//private:
	static Material* CreateFromJson(const char* matPath);
	static unsigned int LoadTexture(const char* path);
};