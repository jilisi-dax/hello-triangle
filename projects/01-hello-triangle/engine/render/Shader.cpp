#include "render/Shader.h"


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

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vCode = processIncludes(readFile(vertexPath), dirOf(vertexPath));
    std::string fCode = processIncludes(readFile(fragmentPath), dirOf(fragmentPath));
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

    glDeleteShader(vertexShader);
    glDeleteShader(fs);

    m_valid = vOk && fOk && checkProgramLink(ID);
}
Shader::~Shader()
{
    glDeleteProgram(ID);
}
std::string Shader::processIncludes(const std::string& code, const std::string& dir)
{
    std::istringstream in(code);
    std::string out, line;
    while (std::getline(in, line))
    {
        if (line.find("#include") != std::string::npos)
        {
            size_t q1 = line.find('"');
            size_t q2 = line.find('"', q1 + 1);
            if (q1 != std::string::npos && q2 != std::string::npos)
                out += readFile((dir + line.substr(q1 + 1, q2 - q1 - 1)).c_str());
            else
                out += line + "\n";   // 残缺 include 原样放行，让 GLSL 编译器报错
        }
        else
            out += line + "\n";
    }
    return out;
}