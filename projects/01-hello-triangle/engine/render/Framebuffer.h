#pragma once
#include "core/pch.h"


class Framebuffer
{
public:
	virtual ~Framebuffer() = default;

	virtual void Begin(int w, int h) = 0;       // 绑 FBO + 设 viewport（尺寸变化自动重建）
	virtual void End() = 0;
	virtual void BindColor(int unit) const = 0; // 颜色附件绑到采样单元（bloom 链采样）
	virtual void BindDepth(int unit) const = 0; // 深度附件绑到采样单元（shadow 采样）
	virtual int Width() const = 0;
	virtual int Height() const = 0;

	// depthOnly=true：无颜色附件（shadow map 模式），清理只清深度
	static Framebuffer* Create(int w, int h, bool depthOnly = false);
};