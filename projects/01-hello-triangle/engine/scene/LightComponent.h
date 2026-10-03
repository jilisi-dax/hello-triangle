#pragma once
#include "scene/SceneObject.h"

class LightComponent : public Component
{
public:
	glm::vec3 lightColor = glm::vec3(1.0f);
	float ambientStrength = 0.2f;
	float range = 10.0f;
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f); // 聚光朝向
	float cutoff = -1.0f;   // 锥角余弦
	float cutoffOuter = -2.0f;  // 全暗边界锥角
	bool castShadow = false; //阴影贴图缓冲标记
};
