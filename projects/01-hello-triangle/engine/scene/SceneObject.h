#pragma once
#include "core/pch.h"

class SceneObject;
class CameraComponent;
class LightComponent;
struct ShadowFrame;
struct EnvFrame;

// 组件基类
class Component
{
public:
	SceneObject* owner = nullptr;

	glm::vec3 GetPos();

	virtual void update(float dt) {}
	virtual void draw(CameraComponent& cam, std::vector<LightComponent*>& lights, const ShadowFrame& shadow, const EnvFrame& env) {}
	virtual void drawDepth(const glm::mat4& lightSpaceMat) {}
	virtual ~Component() {}
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
	virtual void draw(CameraComponent& cam, std::vector<LightComponent*>& lights, const ShadowFrame& shadow, const EnvFrame& env)
	{
		if (!isShow) return;
		for (auto c : components)
			c->draw(cam, lights, shadow, env);
	}
	void drawDepth(const glm::mat4& lightSpaceMat)
	{
		if (!isShow) return;
		for (auto c : components)
			c->drawDepth(lightSpaceMat);
	}
};
