#include "camera.h"
#include "pch.h"

void CameraComponent::updateView()
{
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}
void CameraComponent::updateFront()
{
	cameraFront.x = cos(glm::radians(yaw));
	cameraFront.y = 0.0f;
	cameraFront.z = sin(glm::radians(yaw));
	cameraFront = glm::normalize(cameraFront);
}

void CameraComponent::updateProjection()
{
	projection = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 100.0f);
}

void CameraComponent::moveForward(float speed)
{
	cameraPos += speed * cameraFront;
	updateView();
}

void CameraComponent::moveBackward(float speed)
{
	cameraPos -= speed * cameraFront;
	updateView();
}

void CameraComponent::moveLeft(float speed)
{
	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
	updateView();
}

void CameraComponent::moveRight(float speed)
{
	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
	updateView();
}
void CameraComponent::rotate(float angle)
{
	yaw += angle;
	updateFront();
	updateView();
}

void CameraComponent::zoom(float fovDelta)
{
	fov += fovDelta;
	fov = glm::clamp(fov, 1.0f, 89.0f);
	updateProjection();
}