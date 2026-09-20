#pragma once
#include "pch.h"
#include "Scene.h"
void CameraTick();

class CameraComponent : public Component
{
public:
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 4.0f);
    glm::vec3 cameraFront;
    float yaw = -90.0f;
    float fov = 45.0f;
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view;
    glm::mat4 projection;

    CameraComponent()
    {
        updateFront();
        updateView();
        updateProjection();
    }

    glm::mat4 getCameraView() { return view; }
    glm::mat4 getProjection() { return projection; }

    void moveForward(float speed);
    void moveBackward(float speed);
    void moveLeft(float speed);
    void moveRight(float speed);
    void rotate(float angle);
    void zoom(float fovDelta);

private:
    void updateView();
    void updateFront();
    void updateProjection();
};