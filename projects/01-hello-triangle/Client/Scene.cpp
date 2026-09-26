#include "Scene.h"
#include "TextureCube.h"
#include "ResourceLib.h"
#include "sys.h"

glm::vec3 Component::GetPos() {
    if (owner)
        return owner->GetPos();
    else
        return glm::vec3(0, 0, 0);
}

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
        obj->draw(*m_mainCamera, m_lights ,m_shadow);
}
void Scene::renderDepth(const glm::mat4& lightSpaceMat)
{
    for (auto obj : Objects)
        obj->drawDepth(lightSpaceMat);
}
void Scene::setSkybox(const char* cubemapDir)
{
    m_skybox = ResourceLib::GetSkybox(cubemapDir);
}

void Scene::clear()
{
    for (auto obj : Objects)
        delete obj;
    Objects.clear();
}