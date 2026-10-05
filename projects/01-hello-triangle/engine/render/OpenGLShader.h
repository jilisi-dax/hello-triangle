#pragma once
#include "render/Shader.h"

class OpenGLShader : public Shader
{
public:
	OpenGLShader(const char* vertexPath, const char* fragmentPath);
	~OpenGLShader() override;

	bool IsValid() const override;
	void UseProgram() override;
	void setUniform1i(const char* name, int v) override;
	void setUniform1f(const char* name, float v) override;
	void setUniform3f(const char* name, float x, float y, float z) override;
	void setUniformMat3(const char* name, const glm::mat3& m) override;
	void setUniformMat4(const char* name, const glm::mat4& m) override;

private:
	int loc(const char* name);   // uniform 地址缓存——OpenGL 特有，不外露

	unsigned int ID = 0;
	bool m_valid = false;
	std::unordered_map<std::string, int> m_locCache;

	static std::string readFile(const char* path);
	static std::string dirOf(const std::string& path);
	std::string processIncludes(const std::string& code, const std::string& dir);
};