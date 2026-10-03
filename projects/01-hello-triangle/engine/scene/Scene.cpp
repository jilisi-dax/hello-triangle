#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "scene/LightComponent.h"
#include "render/TextureCube.h"
#include "render/ResourceLib.h"
#include "platform/Paths.h"

Scene::~Scene()
{
    for (auto o : Objects)
        delete o;
    Objects.clear();
}

void Scene::add(SceneObject* obj)
{
    Objects.push_back(obj);
}

void Scene::update(float dt)
{
    for (auto obj : Objects)
        obj->update(dt);
}

std::vector<LightComponent*>& Scene::collectLights()
{
    m_lights.clear();
    for (auto obj : Objects)
        for (auto c : obj->components)
            if (LightComponent* cL = dynamic_cast<LightComponent*>(c))
                m_lights.push_back(cL);
    return m_lights;
}

void Scene::render()
{
    collectLights();
    for (auto obj : Objects)
        obj->draw(*m_mainCamera, m_lights, m_shadow, m_env);
}
void Scene::renderDepth(const glm::mat4& lightSpaceMat)
{
    for (auto obj : Objects)
        obj->drawDepth(lightSpaceMat);
}
void Scene::setSkybox(const char* cubemapDir)
{
    m_skybox = ResourceLib::GetSkybox(cubemapDir);
    TextureCube* irr = ResourceLib::GetIrradiance(cubemapDir);
    TextureCube* pre = ResourceLib::GetPrefiltered(cubemapDir);
    m_env.enabled = (irr && pre);
    if (irr) m_env.irradianceTex = irr->GetID();
    if (pre) m_env.prefilteredTex = pre->GetID();
    m_env.brdfLutTex = ResourceLib::GetBrdfLut();
}

void Scene::clear()
{
    for (auto obj : Objects)
        delete obj;
    Objects.clear();
}