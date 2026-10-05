#pragma once
#include "core/pch.h"

// 着色器程序（抽象）：读文件/include 展开/编译链接/uniform 上传全归后端
class Shader
{
public:
	virtual ~Shader() = default;

	virtual bool IsValid() const = 0;
	virtual void UseProgram() = 0;
	virtual void setUniform1i(const char* name, int v) = 0;
	virtual void setUniform1f(const char* name, float v) = 0;
	virtual void setUniform3f(const char* name, float x, float y, float z) = 0;
	virtual void setUniformMat3(const char* name, const glm::mat3& m) = 0;
	virtual void setUniformMat4(const char* name, const glm::mat4& m) = 0;

	// 工厂：选择后端的唯一入口
	static Shader* Create(const char* vertexPath, const char* fragmentPath);
};