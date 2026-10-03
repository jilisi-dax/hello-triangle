#pragma once
#include "core/pch.h"

class SceneObject;
class CameraComponent;
class LightComponent;
class TextureCube;

struct ShadowFrame
{
	bool enabled = false;
	glm::mat4 lightSpaceMat = glm::mat4(1.0f);
	glm::vec3 lightDir = glm::vec3(0.0f, -1.0f, 0.0f);
	unsigned int depthTex = 0;
};

struct EnvFrame
{
	bool enabled = false;
	unsigned int irradianceTex = 0;   // 漫反射环境（辐照度图）
	unsigned int prefilteredTex = 0;  // 镜面环境（预滤波图）
	unsigned int brdfLutTex = 0;

};

class Scene
{
public:
	Scene() {};
	~Scene();

	std::string neme = "";
	std::vector < SceneObject*> Objects;
private:
	CameraComponent* m_mainCamera = nullptr;
	std::vector<LightComponent*> m_lights;
	ShadowFrame m_shadow;
	TextureCube* m_skybox = nullptr;
	EnvFrame m_env;
public:
	std::vector<LightComponent*>& collectLights();
	void setMainCamera(CameraComponent* cam) { m_mainCamera = cam; }
	CameraComponent* mainCamera() { return m_mainCamera; }

	void setSkybox(const char* cubemapDir);
	TextureCube* skybox() const { return m_skybox; }
	EnvFrame& env() { return m_env; }

	void add(SceneObject* obj);
	void remove(SceneObject* obj);

	void update(float dt);
	void render();
	void renderDepth(const glm::mat4& lightSpaceMat);

	ShadowFrame& shadowInfo() { return m_shadow; }

	void clear();
};
