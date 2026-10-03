#include "scene/SceneObject.h"

glm::vec3 Component::GetPos() {
    if (owner)
        return owner->GetPos();
    else
        return glm::vec3(0, 0, 0);
}
