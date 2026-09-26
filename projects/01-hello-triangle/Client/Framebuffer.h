#pragma once
#include "pch.h"

class Framebuffer
{
public:
	Framebuffer(int width, int height);
	~Framebuffer();

	void Begin(int width, int height);
	void End();

	unsigned int GetColorTexture() const { return m_colorTex; }

	static void BlitTo(const Framebuffer* src, const Framebuffer* dst);

private:
	void Rebuild(int width, int height);
	void Destroy();

	unsigned int m_fbo = 0;
	unsigned int m_colorTex = 0;
	unsigned int m_depthRbo = 0;
	int m_width = 0;
	int m_height = 0;
};





