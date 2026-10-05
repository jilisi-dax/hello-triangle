#pragma once
#include "core/pch.h"


class Mesh
{
public:
	virtual ~Mesh() = default;
	virtual void draw() const = 0;
	virtual int VertexCount() const = 0;

	// 工厂：顶点数学在公共层算好，上传交给当前后端实现
	static Mesh* CreateCube();
	static Mesh* CreateGround();
	static Mesh* CreateSphere(int stacks = 48, int slices = 96);
	static Mesh* LoadOBJ(const char* path);
	static bool ParseOBJ(const char* path, std::vector<float>& vertices);
};