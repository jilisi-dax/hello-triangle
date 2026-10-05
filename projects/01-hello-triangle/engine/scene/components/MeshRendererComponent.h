#pragma once
#include "core/pch.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "render/Mesh.h"
#include "render/Material.h"
#include "platform/Paths.h"
#include "render/ResourceLib.h"
class MeshRendererComponent : public Component
{
public:

	Mesh* m_mesh = nullptr;
	Material* m_material = nullptr;

	void draw(CameraComponent& cam, std::vector<LightComponent*>& lights, const ShadowFrame& shadow, const EnvFrame& env) override;
	void drawDepth(const glm::mat4& lightSpaceMat) override;
};

class ModelRenderer : public MeshRendererComponent
{
public:
	std::string m_meshPath;
	std::string m_texturePath;
	ModelRenderer()
	{
		m_material = ResourceLib::GetMaterial("mat/role.mat");
		m_meshPath = "role/Forest_Spr/Forest_Spr_50k.obj";
		m_mesh = ResourceLib::GetMesh(m_meshPath.c_str());
		if (!m_mesh) { LOG_ERROR("Mesh load error: %s", m_meshPath.c_str()); return; }
		LOG_INFO("Mesh vertices count: %d", m_mesh->VertexCount());
	}
};


class MeshRenderer : public MeshRendererComponent
{
public:
	MeshRenderer(const char* matKey = "mat/plastic.mat", const char* meshKey = "builtin:cube")
	{
		m_material = ResourceLib::GetMaterial(matKey);
		m_mesh = ResourceLib::GetMesh(meshKey);
	}
};


class GroundRenderer : public MeshRendererComponent
{
public:
	GroundRenderer()
	{
		m_material = ResourceLib::GetMaterial("mat/ground.mat");
		m_mesh = ResourceLib::GetMesh("builtin:ground");
	}
};

class SpinComponent : public Component
{
public:
	glm::vec3 axis = glm::vec3(1.0f, -1.0f, 0.0f);
	float speed = 45.0f;
	float angle = 0.0f; 

	void update(float dt) override
	{
		angle += speed * dt;
		glm::vec3 r = owner->GetRotation();
		r.y = angle;
		owner->SetRotation(r);
	}
};