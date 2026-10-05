#pragma once
#include "render/RendererAPI.h"

class OpenGLRendererAPI : public RendererAPI
{
public:
	void Init() override;
	void SetViewport(int x, int y, int w, int h) override;
	void SetClearColor(float r, float g, float b, float a) override;
	void ClearColorDepth() override;
	void SetDepthTest(bool on) override;
	void SetDepthFunc(DepthFunc f) override;
	void SetCullFace(bool on, CullMode mode) override;
	void SetBlend(bool on) override;
	void SetBlendFunc(BlendFactor src, BlendFactor dst) override;
};