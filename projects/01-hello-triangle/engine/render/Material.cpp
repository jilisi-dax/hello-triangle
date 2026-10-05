#include "render/Material.h"
#include "render/ResourceLib.h"
#include "render/Texture2D.h"
#include "platform/Paths.h"

void Material::bind()
{
    m_shader->UseProgram();

    if (diffuseMap)
        diffuseMap->Bind(0);
    m_shader->setUniform1i("ourTexture", 0);

    m_shader->setUniform1f("cellSize", cellSize);
    m_shader->setUniform1i("albedoMode", albedoMode);

    m_shader->setUniform1i("normalMapEnabled", normalMap ? 1 : 0);
    m_shader->setUniform1i("normalMap", 2);
    if (normalMap)
    {
        normalMap->Bind(2);   // 0 号是漫反射、1 号阴影深度图，法线贴图 2 号
    }

    m_shader->setUniform3f("materialColor", color.r, color.g, color.b);
    m_shader->setUniform1f("shininess", shininess);
    m_shader->setUniform1f("specularStrength", specularStrength);
    m_shader->setUniform1f("metallic", metallic);
    m_shader->setUniform1f("roughness", roughness);
    m_shader->setUniform1i("metallicMapEnabled", metallicMap ? 1 : 0);
    m_shader->setUniform1i("metallicMap", 3);
    if (metallicMap)
    {
        metallicMap->Bind(3);
    }

    m_shader->setUniform1i("roughnessMapEnabled", roughnessMap ? 1 : 0);
    m_shader->setUniform1i("roughnessMap", 4);
    if (roughnessMap)
    {
        roughnessMap->Bind(4);
    }
    m_shader->setUniform1i("aoMapEnabled", aoMap ? 1 : 0);
    m_shader->setUniform1i("aoMap", 8);
    if (aoMap)
    {
        aoMap->Bind(8);
    }

    m_shader->setUniform1i("ormMapEnabled", ormMap ? 1 : 0);
    m_shader->setUniform1i("ormMap", 9);
    if (ormMap)
    {
        ormMap->Bind(9);
    }
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
    if (j.contains("normalMap"))
        m->normalMap = ResourceLib::GetTexture(j["normalMap"].get<std::string>().c_str());
    if (j.contains("color"))
        m->color = glm::vec3(j["color"][0], j["color"][1], j["color"][2]);
    m->shininess = j.value("shininess", 32.0f);
    m->specularStrength = j.value("specularStrength", 0.5f);
    m->albedoMode = j.value("albedoMode", 0);
    if (j.contains("aoMap"))
        m->aoMap = ResourceLib::GetTexture(j["aoMap"].get<std::string>().c_str());
    m->metallic = j.value("metallic", 0.0f);
    m->roughness = j.value("roughness", 0.5f);
    if (j.contains("metallicMap"))
        m->metallicMap = ResourceLib::GetTexture(j["metallicMap"].get<std::string>().c_str());
    if (j.contains("roughnessMap"))
        m->roughnessMap = ResourceLib::GetTexture(j["roughnessMap"].get<std::string>().c_str());
    if (j.contains("ormMap"))
        m->ormMap = ResourceLib::GetTexture(j["ormMap"].get<std::string>().c_str());
    return m;
}