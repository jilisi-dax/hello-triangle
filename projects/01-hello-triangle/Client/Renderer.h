#pragma once
#include "pch.h"
#include "ShadowMap.h"
#include <memory>
#include "Framebuffer.h"

class Scene;

class Renderer
{
public:
	Renderer();
	~Renderer();


	void RenderFrame(Scene& scene, int fbw, int fbh);


private:
	void RenderShadowMap(Scene& scene);
	void DrawDebugDepth();
	void RenderPostEffect();
	void RenderSkybox(Scene& scene);

	std::unique_ptr<ShadowMap> m_shadowMap;
	std::unique_ptr<Framebuffer> m_framebuffer;
	unsigned int m_quadVAO = 0;
	unsigned int m_quadVBO = 0;

	int m_effectMode = 0;   // 0=原样 1=灰度 2=反色 3=锐化
public:
	void ToggleEffectMode(int m) { m_effectMode = (m_effectMode == m) ? 0 : m; }
private:

};