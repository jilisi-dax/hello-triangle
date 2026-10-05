#pragma once

class Scene;
class CameraComponent;
struct GLFWwindow;

extern Scene scene;
extern CameraComponent* cam;
extern CameraComponent* g_mainCamera;

void init(Scene& scene, GLFWwindow* window);
void processInput(CameraComponent& camera, float deltaTime);