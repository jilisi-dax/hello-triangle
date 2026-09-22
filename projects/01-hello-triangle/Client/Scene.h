#pragma once
#include "pch.h"

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
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f); // 聚光朝向
	float cutoff = -1.0f;   // 锥角余弦
	float cutoffOuter = -2.0f;  // 全暗边界锥角

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
	SceneObject() {}
	virtual ~SceneObject()
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
	void SetScale(glm::vec3 s) { scale = s; }
	glm::vec3 GetRotation() { return rotation; }
	void SetRotation(glm::vec3 r) { rotation = r; }

	glm::mat4 getModelMatrix() const
	{
		glm::mat4 m = glm::mat4(1.0f);
		m = glm::translate(m, m_position);
		m = glm::rotate(m, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		m = glm::rotate(m, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		m = glm::rotate(m, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		m = glm::scale(m, scale);
		return m;
	}

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