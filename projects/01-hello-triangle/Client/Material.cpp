#include "Material.h"
#include "ResourceLib.h"
#include <sys.h>

void Material::bind()
{
    glUseProgram(m_shader->getProgram());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glUniform1i(glGetUniformLocation(m_shader->getProgram(), "ourTexture"), 0);

    glUniform1f(glGetUniformLocation(m_shader->getProgram(), "cellSize"), cellSize);

    glUniform3f(glGetUniformLocation(m_shader->getProgram(), "materialColor"), color.r, color.g, color.b);
    glUniform1f(glGetUniformLocation(m_shader->getProgram(), "shininess"), shininess);
    glUniform1f(glGetUniformLocation(m_shader->getProgram(), "specularStrength"), specularStrength);
}

unsigned int Material::LoadTexture(const char* path)
{
    unsigned int tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int w, h, ch;
    unsigned char* data = stbi_load(path, &w, &h, &ch, 0);
    if (data)
    {
        GLenum fmt = (ch == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        LOG_ERROR("Texture load error: %s", path);
        glDeleteTextures(1, &tex);
        tex = 0;   // 失败返回 0，调用方才拦得住
    }
    stbi_image_free(data);
    return tex;

}

Material* Material::CreateFromJson(const char* matPath)
{
    std::ifstream file(getAssetPath() + matPath);
    if (!file.is_open())
    {
        LOG_ERROR("Material file not found: %s", matPath);
        return nullptr;
    }

    nlohmann::json j;
    try { file >> j; }
    catch (const nlohmann::json::parse_error& e)
    {
        LOG_ERROR("Material JSON parse error in %s: %s", matPath, e.what());
        return nullptr;
    }

    Material* m = new Material();

    std::string vert = getAssetPath() + j.value("vertex", "shaders/cube.vert");
    std::string frag = getAssetPath()  + j.value("fragment", "shaders/cube.frag");
    m->m_shader = ResourceLib::GetShader(vert.c_str(), frag.c_str());
    if (!m->m_shader) {
        delete m;
        return nullptr;
    }
    if (j.contains("texture"))
    {
        std::string tex = j["texture"].get<std::string>();
        m->diffuseMap = ResourceLib::GetTexture(tex.c_str());
    }
    if (j.contains("color"))
        m->color = glm::vec3(j["color"][0], j["color"][1], j["color"][2]);
    m->shininess = j.value("shininess", 32.0f);
    m->specularStrength = j.value("specularStrength", 0.5f);
    return m;
}