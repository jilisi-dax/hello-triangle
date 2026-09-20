#pragma once
#include "pch.h"
#include "Scene.h"
class MeshRendererComponent : public Component
{
public:
	~MeshRendererComponent()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteTextures(1, &m_texture);
		delete m_shader;
	}

	unsigned int VAO, VBO;
	unsigned int m_texture;
	Shader* m_shader = nullptr;
	int vertexCount = 0;
	virtual void init() = 0;
	virtual void onDraw() = 0;
	void draw(CameraComponent& cam, std::vector<LightComponent*>& lights) override
	{
		if (!m_shader) return;
		glUseProgram(m_shader->getProgram());

		glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(cam.getCameraView()));
		glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(cam.getProjection()));

		glm::mat4 model = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glUniform1i(glGetUniformLocation(m_shader->getProgram(), "ourTexture"), 0);
		// 用收集到的灯
		int count = 0;
		if (!lights.empty())
		{
			float objPosX = 0, objPosY = 0, objPosZ = 0;  // 先写死，后面改成从 owner 读

			for (int i = 0; i < lights.size() && count < 8; i++)
			{
				float dx = lights[i]->GetPos().x - objPosX;
				float dy = lights[i]->GetPos().y - objPosY;
				float dz = lights[i]->GetPos().z - objPosZ;
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

		glBindVertexArray(VAO);
		onDraw();
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
		m_meshPath = getAssetPath() + "role/Forest_Spr/Forest_Spr_5k.obj";
		m_texturePath = getAssetPath() + "role/Forest_Spr/Forest_Spr.png";
		init();
	}

	void init() override
	{
		std::vector<float> modelVertices;
		if (!loadOBJ(m_meshPath.c_str(), modelVertices))
		{
			printf("Mesh load error \n");
			return;
		}
		vertexCount = (int)modelVertices.size() / 8;
		printf("Mesh vertices count : %d\n", vertexCount);
		//unsigned int modelVAO, modelVBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(float), modelVertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);

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
	void onDraw() override
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 0.5f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
};

class CubeRenderer : public MeshRendererComponent
{
	unsigned int EBO;
public:
	CubeRenderer()
	{
		m_shader = new Shader("../../shaders/cube.vert", "../../shaders/cube.frag");
		init();
	}

	void init() override
	{
		float cubeVertices[192] = {
			// 后(-Z)，法线(0,0,-1)
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
			// 前(+Z)，法线(0,0,1)
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f, 1.0f,
			// 左(-X)，法线(-1,0,0)
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
			// 右(+X)，法线(1,0,0)
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
			 // 下(-Y)，法线(0,-1,0)
			 -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			  0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			  0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
			 -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
			 // 上(+Y)，法线(0,1,0)
			 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 0.0f,
			  0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
			  0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 1.0f,
			 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
		};
		unsigned int cubeIndices[36] = {
			0,1,2, 0,2,3,      // 后
			4,5,6, 4,6,7,      // 前
			8,9,10, 8,10,11,   // 左
			12,13,14, 12,14,15,// 右
			16,17,18, 16,18,19,// 下
			20,21,22, 20,22,23 // 上
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);

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

		vertexCount = 36;
	}
	void onDraw() override
	{
		glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);   // 立方体用索引
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
		float groundVertices[] = {
				-50.0f, -2.0f, -50.0f,
				 50.0f, -2.0f, -50.0f,
				 50.0f, -2.0f,  50.0f,
				 50.0f, -2.0f,  50.0f,
				-50.0f, -2.0f,  50.0f,
				-50.0f, -2.0f, -50.0f
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}
	void onDraw() override
	{
		glUniform1f(glGetUniformLocation(m_shader->getProgram(), "cellSize"), 2.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};