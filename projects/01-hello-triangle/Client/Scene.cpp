#include "Scene.h"


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
void Scene::render()
{
    std::vector<LightComponent*> lights;
    for (auto obj : Objects)
    {
        for (auto c : obj->components)
        {
            if (LightComponent * cL = dynamic_cast<LightComponent*>(c))
                lights.push_back(dynamic_cast<LightComponent*>(cL));
        }
    }

    for (auto obj : Objects)
        obj->draw(*m_mainCamera, lights);
}
void Scene::clear()
{
    for (auto obj : Objects)
        delete obj;
    Objects.clear();
}