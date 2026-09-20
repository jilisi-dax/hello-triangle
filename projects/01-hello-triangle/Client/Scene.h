#pragma once
#include "pch.h"
#include "Shader.h"

class SceneObject;
class CameraComponent;
class LightComponent;

class Component
{
public:
	SceneObject* owner = nullptr;   // 我挂在谁身上

	glm::vec3 GetPos();

	virtual void update(float dt) {}
	virtual void draw(CameraComponent& cam, std::vector<LightComponent*>& lights) {}
	virtual ~Component() {}
};
class LightComponent : public Component
{
public:
	glm::vec3 lightColor = glm::vec3(1.0f);
	float ambientStrength = 0.2f;
	float range = 10.0f;
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
public:
	void setMainCamera(CameraComponent* cam) { m_mainCamera = cam; }
public:
	void add(SceneObject* obj);
	void remove(SceneObject* obj);

	void update(float dt);
	void render();

	void clear();
};


class SceneObject
{
public:
	SceneObject() {};
	~SceneObject()
	{
		for (auto c : components)
			delete c;
	}
protected:
	std::string name = "";
	bool active = true;
	bool isShow = true;
	glm::vec3 m_position;
	glm::vec3 rotation;
	glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);

public:
	void SetPos(glm::vec3 position) { m_position = position; };
	glm::vec3 GetPos() { return m_position; };
	std::vector<Component*> components;
	void addComponent(Component* c)
	{
		c->owner = this;
		components.push_back(c);
	}
	void update(float dt)
	{
		if (!active) return;
		for (auto c : components)
			c->update(dt);
	}

	//virtual void update(float dt) {}    // 每帧更新逻辑（空的，子类重写）
	void draw(CameraComponent& cam, std::vector<LightComponent*>& lights)
	{
		if (!isShow) return;
		for (auto c : components)
			c->draw(cam, lights);
	}

};