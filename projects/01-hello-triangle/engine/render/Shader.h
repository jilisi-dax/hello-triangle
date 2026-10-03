
#pragma once
#define GLAD_APIENTRY_DEFINED
#include <glad/glad.h>

#include "core/pch.h"
#include "core/Log.h"

class Shader {
public:
    unsigned int ID;
    bool m_valid = false;
private:
    std::unordered_map<std::string, int> m_locCache;
public:
    // 传入两个文件路径，构造时自动完成 读文件→编译→链接
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void UseProgram() { glUseProgram(ID); }
    int getProgram() { return ID; }
    int loc(const char* name)
    {
        auto it = m_locCache.find(name);
        if (it != m_locCache.end()) return it->second;
        int l = glGetUniformLocation(ID, name);
        m_locCache[name] = l;
        return l;
    }
    void setUniform1f(const char* name, float v) { glUniform1f(glGetUniformLocation(ID, name), v); }
    void setUniform1i(const char* name, int v) { glUniform1i(glGetUniformLocation(ID, name), v); }
    void setUniform3f(const char* name, float x, float y, float z)
    {
        glUniform3f(glGetUniformLocation(ID, name), x, y, z);
    }
    void setUniformMat4(const char* name, const glm::mat4& m)
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(m));
    }

private:
    std::string readFile(const char* path)
    {
        std::ifstream f(path);
        if (!f.is_open())
        {
            LOG_ERROR("Shader file not found: %s", path);
            return "";
        }
        std::stringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }

    std::string dirOf(const std::string& path)
    {
        size_t p = path.find_last_of("/\\");
        return (p == std::string::npos) ? "" : path.substr(0, p + 1);
    }
    std::string processIncludes(const std::string& code, const std::string& dir);
};
