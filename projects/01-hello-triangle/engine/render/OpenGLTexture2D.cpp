#include "render/OpenGLTexture2D.h"
#include "render/Shader.h"
#include "render/ResourceLib.h"
#include "platform/Paths.h"
#include "core/Log.h"
#include <glad/glad.h>

OpenGLTexture2D::OpenGLTexture2D(const char* path)
{
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int ch;
	unsigned char* data = stbi_load(path, &m_w, &m_h, &ch, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (data)
	{
		GLenum fmt = (ch == 4) ? GL_RGBA : (ch == 3) ? GL_RGB : (ch == 1) ? GL_RED : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, fmt, m_w, m_h, 0, fmt, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		LOG_ERROR("Texture load error: %s", path);
		glDeleteTextures(1, &m_id);
		m_id = 0;   // 失败哨兵不变，IsValid 拦得住
	}
	stbi_image_free(data);
}

OpenGLTexture2D::OpenGLTexture2D(unsigned int prebuiltId, int w, int h)
	: m_id(prebuiltId), m_w(w), m_h(h)
{
}

Texture2D* Texture2D::Create(const char* path)
{
	return new OpenGLTexture2D(path);
}


Texture2D* Texture2D::CreateBrdfLut()
{
	Shader* s = ResourceLib::GetShader(
		(getAssetPath() + "shaders/post.vert").c_str(),
		(getAssetPath() + "shaders/util_brdf_lut.frag").c_str()
	);
	if (!s) return nullptr;

	float quad[] = {
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
	};
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	unsigned int tex, fbo;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	glViewport(0, 0, 512, 512);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	s->UseProgram();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteFramebuffers(1, &fbo);

	LOG_INFO("BrdfLut baked: 512x512");
	return new OpenGLTexture2D(tex, 512, 512);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	if (m_id) glDeleteTextures(1, &m_id);
}

void OpenGLTexture2D::Bind(int unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_id);
}