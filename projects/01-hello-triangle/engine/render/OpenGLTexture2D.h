#pragma once
#include "render/Texture2D.h"

class OpenGLTexture2D : public Texture2D
{
public:
	OpenGLTexture2D(const char* path);
	OpenGLTexture2D(unsigned int prebuiltId, int w, int h);
	~OpenGLTexture2D() override;

	void Bind(int unit) const override;
	bool IsValid() const override { return m_id != 0; }
	int Width() const override { return m_w; }
	int Height() const override { return m_h; }

private:
	unsigned int m_id = 0;
	int m_w = 0, m_h = 0;
};