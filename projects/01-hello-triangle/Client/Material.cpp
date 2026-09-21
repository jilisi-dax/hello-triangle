#include "Material.h"
#include "ResourceLib.h"

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
    }
    stbi_image_free(data);
    return tex;

}

Material* Material::CreateTextured(const char* vertPath, const char* fragPath, const char* texPath)
{
    Material* m = new Material();
    m->m_shader = ResourceLib::GetShader(vertPath, fragPath);
    m->diffuseMap = ResourceLib::GetTexture(texPath);
    return m;
}

Material* Material::CreateGrid(const char* vertPath, const char* fragPath)
{
    Material* m = new Material();
    m->m_shader = new Shader(vertPath, fragPath);
    return m;
}
