#include "keyBoard.h"
#include "ActionMap.h" 

void processInput(CameraComponent& camera, float deltaTime)
{

	if (ActionMap::IsDown(GameAction::Exit))
		ActionMap::RequestExit();

	float speed = 2.5f * deltaTime;
	if (ActionMap::IsDown(GameAction::MoveForward))  camera.moveForward(speed);
	if (ActionMap::IsDown(GameAction::MoveBackward)) camera.moveBackward(speed);
	if (ActionMap::IsDown(GameAction::MoveLeft))     camera.moveLeft(speed);
	if (ActionMap::IsDown(GameAction::MoveRight))    camera.moveRight(speed);

	const float mouseSensitivity = 0.1f;
	glm::vec2 d = ActionMap::GetLook();
	camera.rotate(d.x * mouseSensitivity, -d.y * mouseSensitivity);

	camera.zoom(ActionMap::GetZoom());

}
