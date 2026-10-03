#include "TextureCube.h"
#include "sys.h"
#include "Shader.h"
#include "ResourceLib.h"
#include "mesh.h"

static unsigned int LoadEquirectHDR(const char* path, int& w, int& h)
{
	int ch;
	float* data = stbi_loadf(path, &w, &h, &ch, 0);
	if (!data)
	{
		LOG_ERROR("EquirectHDR load error: %s", path);
		return 0;
	}
	unsigned int tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, data);

	stbi_image_free(data);
	LOG_INFO("EquirectHDR ready: %dx%d", w, h);
	return tex;
}

TextureCube* TextureCube::CreateFromEquirect(const char* equirectPath)
{
	int ew, eh;
	unsigned int srcTex = LoadEquirectHDR((getAssetPath() + equirectPath).c_str(), ew, eh);
	if (!srcTex) return nullptr;

	const int face = ew / 4;

	unsigned int cubeTex;
	glGenTextures(1, &cubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F,
			face, face, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	Shader* conv = ResourceLib::GetShader(
		(getAssetPath() + "shaders/util_equirect2cube.vert").c_str(),
		(getAssetPath() + "shaders/util_equirect2cube.frag").c_str());
	Mesh* cube = ResourceLib::GetMesh("builtin:cube");

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, cubeTex, 0);
	if (!conv || !cube || glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("Equirect convert setup failed: %s", equirectPath);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &cubeTex);
		glDeleteTextures(1, &srcTex);
		return nullptr;
	}

	glm::mat4 captureProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::vec3 O(0.0f);
	glm::mat4 captureViews[6] = {
		glm::lookAt(O, glm::vec3(1, 0, 0), glm::vec3(0,-1, 0)),
		glm::lookAt(O, glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)),
		glm::lookAt(O, glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
		glm::lookAt(O, glm::vec3(0,-1, 0), glm::vec3(0, 0,-1)),
		glm::lookAt(O, glm::vec3(0, 0, 1), glm::vec3(0,-1, 0)),
		glm::lookAt(O, glm::vec3(0, 0,-1), glm::vec3(0,-1, 0)),
	};

	glViewport(0, 0, face, face);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 6; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeTex, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		conv->UseProgram();
		conv->setUniformMat4("projection", captureProj);
		conv->setUniformMat4("view", captureViews[i]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, srcTex);
		glUniform1i(glGetUniformLocation(conv->getProgram(), "equirect"), 0);
		cube->draw();
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &srcTex);
	glCullFace(GL_BACK);

	LOG_INFO("Skybox converted: %s (%d px/face)", equirectPath, face);
	return new TextureCube(cubeTex);
}

TextureCube* TextureCube::CreateIrradiance(unsigned int envCubeId, int faceSize)
{
	unsigned int cubeTex;
	glGenTextures(1, &cubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F,
			faceSize, faceSize, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	Shader* conv = ResourceLib::GetShader(
		(getAssetPath() + "shaders/util_equirect2cube.vert").c_str(),
		(getAssetPath() + "shaders/util_irradiance.frag").c_str());
	Mesh* cube = ResourceLib::GetMesh("builtin:cube");

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, cubeTex, 0);
	if (!conv || !cube || glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("Irradiance bake setup failed");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &cubeTex);
		return nullptr;
	}

	glm::mat4 captureProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::vec3 O(0.0f);
	glm::mat4 captureViews[6] = {
		glm::lookAt(O, glm::vec3(1, 0, 0), glm::vec3(0,-1, 0)),
		glm::lookAt(O, glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)),
		glm::lookAt(O, glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
		glm::lookAt(O, glm::vec3(0,-1, 0), glm::vec3(0, 0,-1)),
		glm::lookAt(O, glm::vec3(0, 0, 1), glm::vec3(0,-1, 0)),
		glm::lookAt(O, glm::vec3(0, 0,-1), glm::vec3(0,-1, 0)),
	};

	glViewport(0, 0, faceSize, faceSize);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < 6; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeTex, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		conv->UseProgram();
		conv->setUniformMat4("projection", captureProj);
		conv->setUniformMat4("view", captureViews[i]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeId);
		glUniform1i(glGetUniformLocation(conv->getProgram(), "environment"), 0);
		cube->draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
	glCullFace(GL_BACK);

	LOG_INFO("Irradiance baked: %d px/face", faceSize);
	return new TextureCube(cubeTex);
}

TextureCube* TextureCube::CreatePrefiltered(unsigned int envCubeId, int faceSize)
{
	unsigned int cubeTex;
	glGenTextures(1, &cubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 4);   // 只要 5 层：roughness 0~1
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F,
			faceSize, faceSize, 0, GL_RGBA, GL_FLOAT, nullptr);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);   // 分配 mip 链（128→8，共 5 层）
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	Shader* conv = ResourceLib::GetShader(
		(getAssetPath() + "shaders/util_equirect2cube.vert").c_str(),
		(getAssetPath() + "shaders/util_prefilter.frag").c_str());
	Mesh* cube = ResourceLib::GetMesh("builtin:cube");

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, cubeTex, 0);
	if (!conv || !cube || glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("Prefilter bake setup failed");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &cubeTex);
		return nullptr;
	}

	glm::mat4 captureProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::vec3 O(0.0f);
	glm::mat4 captureViews[6] = {
		glm::lookAt(O, glm::vec3(1, 0, 0), glm::vec3(0,-1, 0)),
		glm::lookAt(O, glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)),
		glm::lookAt(O, glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
		glm::lookAt(O, glm::vec3(0,-1, 0), glm::vec3(0, 0,-1)),
		glm::lookAt(O, glm::vec3(0, 0, 1), glm::vec3(0,-1, 0)),
		glm::lookAt(O, glm::vec3(0, 0,-1), glm::vec3(0,-1, 0)),
	};

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	const int maxMips = 5;
	for (int mip = 0; mip < maxMips; mip++)
	{
		int mipSize = faceSize >> mip;
		float roughness = (float)mip / (float)(maxMips - 1);   // 层号 -> roughness 0~1
		glViewport(0, 0, mipSize, mipSize);
		for (int i = 0; i < 6; i++)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeTex, mip);   // 关键：挂到 mip 层
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			conv->UseProgram();
			conv->setUniformMat4("projection", captureProj);
			conv->setUniformMat4("view", captureViews[i]);
			glUniform1f(glGetUniformLocation(conv->getProgram(), "roughness"), roughness);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeId);
			glUniform1i(glGetUniformLocation(conv->getProgram(), "environment"), 0);
			cube->draw();
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
	glCullFace(GL_BACK);

	LOG_INFO("Prefiltered baked: %d px/face, 5 mips", faceSize);
	return new TextureCube(cubeTex);
}

TextureCube::TextureCube(const char* const faces[6])
{
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

	int w, h, ch;
	for (int i = 0; i < 6; i++)
	{
		unsigned char* data = stbi_load((getAssetPath() + faces[i]).c_str(), &w, &h, &ch, 0);
		if (!data)
		{
			LOG_ERROR("TextureCube face load error: %s", faces[i]);
			glDeleteTextures(1, &m_id);
			m_id = 0;
			return;
		}
		GLenum fmt = (ch == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);

	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	LOG_INFO("TextureCube ready: 6 faces");
}

TextureCube::~TextureCube()
{
	if (m_id) glDeleteTextures(1, &m_id);
}