#include "render/OpenGLFramebuffer.h"
#include "core/Log.h"
#include <glad/glad.h>


OpenGLFramebuffer::OpenGLFramebuffer(int w, int h, bool depthOnly)
{
	m_depthOnly = depthOnly;
	Rebuild(w, h);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
	Destroy();
}
void OpenGLFramebuffer::Begin(int w, int h)
{
	if (w != m_width || h != m_height)
		Rebuild(w, h);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, m_width, m_height);
	if (m_depthOnly)
		glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLFramebuffer::End()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::BindColor(int unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_colorTex);
}

void OpenGLFramebuffer::BindDepth(int unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_depthTex);
}


void OpenGLFramebuffer::Rebuild(int w, int h)
{
	Destroy();
	m_width = w;
	m_height = h;

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	if (!m_depthOnly)
	{
		// 普通画布：RGBA16F 颜色 + RBO 深度（深度只测不采）
		glGenTextures(1, &m_colorTex);
		glBindTexture(GL_TEXTURE_2D, m_colorTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenRenderbuffers(1, &m_depthRbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthRbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRbo);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTex, 0);
	}
	else
	{
		// shadow map：深度是纹理（要采样），无颜色附件
		glGenTextures(1, &m_depthTex);
		glBindTexture(GL_TEXTURE_2D, m_depthTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTex, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG_ERROR("Framebuffer: incomplete %dx%d", m_width, m_height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::Destroy()
{
	if (m_fbo)
		glDeleteFramebuffers(1, &m_fbo);
	if (m_colorTex)
		glDeleteTextures(1, &m_colorTex);
	if (m_depthTex)
		glDeleteTextures(1, &m_depthTex);
	if (m_depthRbo)
		glDeleteRenderbuffers(1, &m_depthRbo);
	m_fbo = 0;
	m_colorTex = 0;
	m_depthTex = 0;
	m_depthRbo = 0;
}

Framebuffer* Framebuffer::Create(int w, int h, bool depthOnly)
{
	return new OpenGLFramebuffer(w, h, depthOnly);
}












