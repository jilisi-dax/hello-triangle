#pragma once
#include "core/pch.h"

class ShadowMap
{
private:
	unsigned int m_fbo = 0;
	unsigned int m_depthTex = 0;
	int m_width = 0;
	int m_height = 0;

public:
	ShadowMap(int width, int height);
	~ShadowMap();

	void Begin();
	void End();

	unsigned int GetDepthTexture() const { return m_depthTex; }


};