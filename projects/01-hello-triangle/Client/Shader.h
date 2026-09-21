
#pragma once
#define GLAD_APIENTRY_DEFINED
#include <glad/glad.h>

#include "pch.h"
#include "log.h"

// 检查着色器程序的链接状态，失败就打印错误日志
inline bool checkProgramLink(unsigned int program)
{
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        LOG_ERROR("着色器程序链接失败：\n%s\n", infoLog);
        return false;
    }
    return true;
}
// 检查单个着色器的编译状态，失败就打印错误日志
inline bool checkShaderCompile(unsigned int shader, const char* name)
{
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        LOG_ERROR("%s 编译失败：\n%s\n", name, infoLog);
        return false;
    }
    return true;
}
class Shader {
public:
    unsigned int ID;
    bool m_valid = false;
    // 传入两个文件路径，构造时自动完成 读文件→编译→链接
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        std::string vCode = readFile(vertexPath);
        std::string fCode = readFile(fragmentPath);
        const char* vSrc = vCode.c_str();
        const char* fSrc = fCode.c_str();

        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vSrc, NULL);
        glCompileShader(vertexShader);
        bool vOk = checkShaderCompile(vertexShader, "顶点着色器");

        unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fSrc, NULL);
        glCompileShader(fs);
        bool fOk = checkShaderCompile(fs, "片段着色器");

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fs);
        glLinkProgram(ID);
        checkProgramLink(ID);

        glDeleteShader(vertexShader);
        glDeleteShader(fs);

        m_valid = vOk && fOk && checkProgramLink(ID);
    }
    ~Shader()
    {
        glDeleteProgram(ID);
    }

    void UseProgram() { glUseProgram(ID); }
    int getProgram() { return ID; }

    void setUniform1f(const char* name, float v) { glUniform1f(glGetUniformLocation(ID, name), v); }
    void setUniform1i(const char* name, int v) { glUniform1i(glGetUniformLocation(ID, name), v); }
    void setUniform3f(const char* name, float x, float y, float z)
    {
        glUniform3f(glGetUniformLocation(ID, name), x, y, z);
    }

private:
    std::string readFile(const char* path)
    {
        std::ifstream f(path);
        std::stringstream ss;
        ss << f.rdbuf();          // 整个文件倒进字符串流
        return ss.str();
    }
};
