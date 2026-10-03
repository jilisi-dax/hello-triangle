#include "Framebuffer.h"

Framebuffer::Framebuffer(int width, int height)
{
	Rebuild(width, height);
}

Framebuffer::~Framebuffer()
{
	Destroy();
}

void Framebuffer::Begin(int width, int height)
{
	if (width != m_width || height != m_height)
		Rebuild(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, m_width, m_height);
}

void Framebuffer::End()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::BlitTo(const Framebuffer* src, const Framebuffer* dst)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst ? dst->m_fbo : 0);
	glBlitFramebuffer(0, 0, src->m_width, src->m_height,
		0, 0, src->m_width, src->m_height,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Framebuffer::Rebuild(int width, int height)
{
	Destroy();
	m_width = width;
	m_height = height;

	glGenFramebuffers(1, &m_fbo);
	glGenTextures(1, &m_colorTex);
	glGenRenderbuffers(1, &m_depthRbo);

	glBindTexture(GL_TEXTURE_2D, m_colorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindRenderbuffer(GL_RENDERBUFFER, m_depthRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG_ERROR("Framebuffer: incomplete %dx%d", m_width, m_height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Destroy()
{
	if (m_fbo)      glDeleteFramebuffers(1, &m_fbo);
	if (m_colorTex) glDeleteTextures(1, &m_colorTex);
	if (m_depthRbo) glDeleteRenderbuffers(1, &m_depthRbo);
	m_fbo = 0; m_colorTex = 0; m_depthRbo = 0;
}





