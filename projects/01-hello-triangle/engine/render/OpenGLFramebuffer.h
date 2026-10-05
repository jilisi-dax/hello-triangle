#pragma once
#include "render/Framebuffer.h"

class OpenGLFramebuffer : public Framebuffer
{
public:
	OpenGLFramebuffer(int w, int h, bool depthOnly);
	~OpenGLFramebuffer() override;

	void Begin(int w, int h) override;
	void End() override;
	void BindColor(int unit) const override;
	void BindDepth(int unit) const override;
	int Width() const override { return m_width; }
	int Height() const override { return m_height; }

private:
	void Rebuild(int w, int h);
	void Destroy();

	unsigned int m_fbo = 0;
	unsigned int m_colorTex = 0;
	unsigned int m_depthRbo = 0;
	unsigned int m_depthTex = 0;
	bool m_depthOnly = false;
	int m_width = 0, m_height = 0;
};