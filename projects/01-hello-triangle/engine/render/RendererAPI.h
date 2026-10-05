#pragma once
#include "core/pch.h"

enum class CullMode { Back, Front};
enum class DepthFunc { Less, LEqual };
enum class BlendFactor { One, SrcAlpha, OneMinusSrcAlpha };

class RendererAPI
{
public:
	virtual ~RendererAPI() = default;

	virtual void Init() = 0;
	virtual void SetViewport(int x, int y, int w, int h) = 0;
	virtual void SetClearColor(float r, float g, float b, float a) = 0;
	virtual void ClearColorDepth() = 0;
	virtual void SetDepthTest(bool on) = 0;
	virtual void SetDepthFunc(DepthFunc f) = 0;
	virtual void SetCullFace(bool on, CullMode mode = CullMode::Back) = 0;
	virtual void SetBlend(bool on) = 0;
	virtual void SetBlendFunc(BlendFactor src, BlendFactor dst) = 0;

	static RendererAPI* Create();
};