#include "render/Shader.h"
#include "render/OpenGLShader.h"

Shader* Shader::Create(const char* vertexPath, const char* fragmentPath)
{
	// API 选择点：将来换/加图形后端只改这里
	return new OpenGLShader(vertexPath, fragmentPath);
}
