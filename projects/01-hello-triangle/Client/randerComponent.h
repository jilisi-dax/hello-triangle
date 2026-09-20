#pragma once
#include "pch.h"
#include "Scene.h"
#include <mesh.h>
class MeshRendererComponent : public Component
{
public:
	~MeshRendererComponent()
	{
		delete m_mesh;
		glDeleteTextures(1, &m_texture);
		delete m_shader;
	}

	Mesh* m_mesh = nullptr;
	unsigned int m_texture = 0;
	Shader* m_shader = nullptr;
	virtual void init() = 0;
	virtual void onDraw() {};
	void draw(CameraComponent& cam, std::vector<LightComponent*>& lights) override
	{
		if (!m_shader) return;
		glUseProgram(m_shader->getProgram());

		glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(cam.getCameraView()));
		glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(cam.getProjection()));

		glm::mat4 model = owner ? owner->getModelMatrix() : glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));

		glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
		glUniformMatrix3fv(glGetUniformLocation(m_shader->getProgram(), "normalMatrix"),
			1, GL_FALSE, glm::value_ptr(normalMat));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glUniform1i(glGetUniformLocation(m_shader->getProgram(), "ourTexture"), 0);
		// 用收集到的灯
		int count = 0;
		if (!lights.empty())
		{
			glm::vec3 objPos = owner ? owner->GetPos() : glm::vec3(0.0f);
			for (int i = 0; i < lights.size() && count < 8; i++)
			{
				float dx = lights[i]->GetPos().x - objPos.x;
				float dy = lights[i]->GetPos().y - objPos.y;
				float dz = lights[i]->GetPos().z - objPos.z;
				float dist = sqrt(dx * dx + dy * dy + dz * dz);

				if (dist < lights[i]->range)
				{
					glUniform3f(glGetUniformLocation(m_shader->getProgram(),
						("lightPos[" + to_string(count) + "]").c_str()),
						lights[i]->GetPos().x, lights[i]->GetPos().y, lights[i]->GetPos().z);

					glUniform3f(glGetUniformLocation(m_shader->getProgram(),
						("lightColor[" + to_string(count) + "]").c_str()),
						lights[i]->lightColor.x, lights[i]->lightColor.y, lights[i]->lightColor.z);

					count++;
				}
			}
		}
		glUniform1i(glGetUniformLocation(m_shader->getProgram(), "lightCount"), count);
		glUniform1f(glGetUniformLocation(m_shader->getProgram(), "time"), (float)glfwGetTime());

		onDraw();
		if (m_mesh) m_mesh->draw();
	}
};

class ModelRenderer : public MeshRendererComponent
{
public:
	string m_meshPath;
	string m_texturePath;
	ModelRenderer()
	{
		m_shader = new Shader("../../shaders/role.vert", "../../shaders/role.frag");
		m_meshPath = getAssetPath() + "role/Forest_Spr/Forest_Spr_50k.obj";
		m_texturePath = getAssetPath() + "role/Forest_Spr/Forest_Spr.png";
		init();
	}

	void init() override
	{
		m_mesh = Mesh::LoadOBJ(m_meshPath.c_str());
		if (!m_mesh) { LOG_ERROR("Mesh load error: %s", m_meshPath.c_str()); return; }
		LOG_INFO("Mesh vertices count: %d", m_mesh->vertexCount);

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int mWidth, mHeight, mNrChannels;
		unsigned char* mData = stbi_load(m_texturePath.c_str(), &mWidth, &mHeight, &mNrChannels, 0);
		if (mData)
		{
			GLenum format = (mNrChannels == 4) ? GL_RGBA : GL_RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, mData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(mData);
	}
	virtual void onDraw() {};
};

class CubeRenderer : public MeshRendererComponent
{
public:
	CubeRenderer()
	{
		m_shader = new Shader("../../shaders/cube.vert", "../../shaders/cube.frag");
		init();
	}
	~CubeRenderer()
	{
	}

	void init() override
	{
		m_mesh = Mesh::CreateCube();

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int w, h, ch;
		unsigned char* data = stbi_load((getAssetPath() + "01.png").c_str(), &w, &h, &ch, 0);
		if (data) {
			GLenum fmt = (ch == 4) ? GL_RGBA : GL_RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);
	}
};
class GroundRenderer : public MeshRendererComponent
{
public:
	GroundRenderer()
	{
		m_shader = new Shader("../../shaders/ground.vert", "../../shaders/ground.frag");
		init();
	}

	void init() override
	{
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