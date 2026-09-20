#include "camera.h"
#include "pch.h"

void CameraComponent::updateView()
{
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}
void CameraComponent::updateFront()
{
	float yawRad = glm::radians(yaw);
	float pitchRad = glm::radians(pitch);
	cameraFront.x = cos(yawRad) * cos(pitchRad);
	cameraFront.y = sin(pitchRad);
	cameraFront.z = sin(yawRad) * cos(pitchRad);
	cameraFront = glm::normalize(cameraFront);
}

void CameraComponent::updateProjection()
{
	projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
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
void CameraComponent::rotate(float yawDelta, float pitchDelta)
{
	yaw += yawDelta;
	pitch += pitchDelta;
	pitch = glm::clamp(pitch, -89.0f, 89.0f);
	updateFront();
	updateView();
}

void CameraComponent::zoom(float fovDelta)
{
	fov += fovDelta;
	fov = glm::clamp(fov, 1.0f, 89.0f);
	updateProjection();
}