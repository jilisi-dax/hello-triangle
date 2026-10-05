#include "render/OpenGLMesh.h"
#include <glad/glad.h>

OpenGLMesh::OpenGLMesh(const std::vector<float>& verts)
{
	build(verts, nullptr);
}

OpenGLMesh::OpenGLMesh(const std::vector<float>& verts, const std::vector<unsigned int>& indices)
{
	build(verts, &indices);
}

OpenGLMesh::~OpenGLMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void OpenGLMesh::build(const std::vector<float>& verts, const std::vector<unsigned int>* indices)
{
	unsigned int stepSize = 11 * sizeof(float);
	vertexCount = (int)verts.size() / 11;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stepSize, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stepSize, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stepSize, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stepSize, (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	if (indices)
	{
		useIndex = true;
		vertexCount = (int)indices->size();
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), indices->data(), GL_STATIC_DRAW);
	}
	glBindVertexArray(0);
}

void OpenGLMesh::draw() const
{
	glBindVertexArray(VAO);
	if (useIndex)
		glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
