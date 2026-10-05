#pragma once
#include "render/Mesh.h"

class OpenGLMesh : public Mesh
{
public:
	OpenGLMesh(const std::vector<float>& verts);
	OpenGLMesh(const std::vector<float>& verts, const std::vector<unsigned int>& indices);
	~OpenGLMesh() override;

	void draw() const override;
	int VertexCount() const override { return vertexCount; }

private:
	void build(const std::vector<float>& verts, const std::vector<unsigned int>* indices);

	unsigned int VAO = 0, VBO = 0, EBO = 0;
	int vertexCount = 0;      // 数组式=顶点数；索引式=索引数
	bool useIndex = false;    // 绘制方式开关
};