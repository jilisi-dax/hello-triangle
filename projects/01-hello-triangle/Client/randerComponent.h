#pragma once
#include "pch.h"
#include "Scene.h"
#include "mesh.h"
#include "Material.h"
#include "sys.h"
class MeshRendererComponent : public Component
{
public:
	~MeshRendererComponent()
	{
		delete m_mesh;
		delete m_material;
	}

	Mesh* m_mesh = nullptr;
	Material* m_material = nullptr;

	void draw(CameraComponent& cam, std::vector<LightComponent*>& lights) override;
};

class ModelRenderer : public MeshRendererComponent
{
public:
	std::string m_meshPath;
	std::string m_texturePath;
	ModelRenderer()
	{
		m_material = Material::CreateTextured("../../shaders/role.vert", "../../shaders/role.frag",
			(getAssetPath() + "role/Forest_Spr/Forest_Spr.png").c_str());
		m_mesh = Mesh::LoadOBJ((getAssetPath() + "role/Forest_Spr/Forest_Spr_50k.obj").c_str());
		if (!m_mesh) { LOG_ERROR("Mesh load error: %s", m_meshPath.c_str()); return; }
		LOG_INFO("Mesh vertices count: %d", m_mesh->vertexCount);
		m_material->shininess = 64.0f;
		m_material->specularStrength = 1.0f;
	}
};


class CubeRenderer : public MeshRendererComponent
{
public:
	CubeRenderer(glm::vec3 color = glm::vec3(1.0f), float shininess = 32.0f, float specStrength = 0.5f)
	{
		m_material = Material::CreateTextured("../../shaders/cube.vert", "../../shaders/cube.frag",
			(getAssetPath() + "02.png").c_str());
		m_material->color = color;
		m_material->shininess = shininess;
		m_mesh = Mesh::CreateCube();
	}
};


class GroundRenderer : public MeshRendererComponent
{
public:
	GroundRenderer()
	{
		m_material = Material::CreateGrid("../../shaders/ground.vert", "../../shaders/ground.frag");
		m_mesh = Mesh::CreateGround();
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