#pragma once
#include "pch.h"

class Mesh
{
public:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    int vertexCount = 0;      // 数组式=顶点数；索引式=索引数
    bool useIndex = false;    // 绘制方式开关

    ~Mesh()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void draw()
    {
        glBindVertexArray(VAO);
        if (useIndex)
            glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

    void build(const std::vector<float>& verts);
    void build(const std::vector<float>& verts, const std::vector<unsigned int>& indices);


    static Mesh* CreateCube();
    static Mesh* CreateGround();

    static Mesh* LoadOBJ(const char* path);
    static bool ParseOBJ(const char* path, std::vector<float>& vertices);
};











