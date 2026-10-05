#pragma once
#include "render/Texture.h"

class Texture2D : public Texture
{
public:
	virtual int Width() const = 0;
	virtual int Height() const = 0;

	// 工厂：图片文件加载（材质贴图）
	static Texture2D* Create(const char* path);

	// 工厂：BRDF LUT 烘焙（FBO 画 512² RG16F，烘焙逻辑在后端实现里）
	static Texture2D* CreateBrdfLut();
};