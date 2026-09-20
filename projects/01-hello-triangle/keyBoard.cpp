#include "keyBoard.h"

void processInput(GLFWwindow* window, CameraComponent& camera, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float speed = 2.5f * deltaTime;   // 每秒 2.5 个单位
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.moveForward(speed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.moveBackward(speed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.moveLeft(speed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.moveRight(speed);
	float rotationSpeed = 60.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) camera.rotate(-rotationSpeed);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.rotate(rotationSpeed);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.zoom(-20.0f * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.zoom(20.0f * deltaTime);
}
