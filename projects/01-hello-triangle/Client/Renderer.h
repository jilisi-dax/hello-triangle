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
	void RenderPostEffect(int w, int h);
	void RenderBloomExtract(int w, int h);
	void RenderBloomDown(int w, int h);
	void RenderBloomUp(int w, int h);
	void RenderSkybox(Scene& scene);

	std::unique_ptr<ShadowMap> m_shadowMap;
	std::unique_ptr<Framebuffer> m_framebuffer;
	std::unique_ptr<Framebuffer> m_bloomA;   // bloom ping-pong A
	std::unique_ptr<Framebuffer> m_bloomB;	// bloom ping-pong B
	std::unique_ptr<Framebuffer> m_bloomMip[4];   // bloom 降采样塔 1/2 ~ 1/16
	unsigned int m_quadVAO = 0;
	unsigned int m_quadVBO = 0;

	int m_effectMode = 0;   // 0=原样 1=灰度 2=反色 3=锐化
	int m_debugEnv = 0; // 0=原样 1=辐照度 2=预滤波；
public:
	void ToggleEffectMode(int m) { m_effectMode = (m_effectMode == m) ? 0 : m; }
	void CycleEnvDebug() { m_debugEnv = (m_debugEnv + 1) % 3; }
private:

};