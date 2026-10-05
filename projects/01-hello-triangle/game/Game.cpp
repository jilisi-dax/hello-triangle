#include "core/pch.h"
#include "Game.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "scene/LightComponent.h"
#include "scene/components/CameraComponent.h"
#include "scene/components/MeshRendererComponent.h"
#include "platform/Input.h"
#include "platform/ActionMap.h"
#include "Role.h"

// 游戏全局状态（F4 上 Application 类时收编）
Scene scene;
CameraComponent* cam = new CameraComponent();
CameraComponent* g_mainCamera = nullptr;

void init(Scene& scene, GLFWwindow* window)
{
	Input::Init(window);
	ActionMap::Init();
	SceneObject* cameraObj = new SceneObject();
	cameraObj->addComponent(cam);
	scene.add(new cubeModel());
	scene.add(new metalCube());
	scene.add(new modelObj());
	scene.add(new ground());
	scene.add(cameraObj);
	scene.setMainCamera(cam);
	//scene.setSkybox("skybox/mountain");
	scene.setSkybox("skybox/polyhaven/venice_sunset_4k.hdr");
	g_mainCamera = cam;
	{
		SceneObject* ball1 = new SceneObject();
		ball1->SetPos(glm::vec3(-3.0f, -0.5f, -2.0f));
		ball1->addComponent(new MeshRenderer("mat/ball_normal.mat", "builtin:sphere"));
		scene.add(ball1);

		SceneObject* ball2 = new SceneObject();
		ball2->SetPos(glm::vec3(0.0f, -0.5f, -4.0f));
		ball2->addComponent(new MeshRenderer("mat/metal.mat", "builtin:sphere"));
		SpinComponent* spin = new SpinComponent();
		spin->axis = glm::vec3(0.0f, 1.0f, 0.0f);
		spin->speed = 30.0f;
		ball2->addComponent(spin);
		scene.add(ball2);

		SceneObject* ball3 = new SceneObject();
		ball3->SetPos(glm::vec3(3.0f, -0.5f, -2.0f));
		ball3->addComponent(new MeshRenderer("mat/plastic.mat", "builtin:sphere"));
		scene.add(ball3);
	}

	{
		SceneObject* lightObj = new SceneObject();
		LightComponent* light = new LightComponent();
		lightObj->SetPos(glm::vec3(0.0f, 0.5f, 8.0f));
		light->lightColor = glm::vec3(5.0f);
		light->range = 20.0f;
		lightObj->addComponent(light);
		scene.add(lightObj);
		// 灯 2
		SceneObject* light2 = new SceneObject();
		LightComponent* l2 = new LightComponent();
		light2->SetPos(glm::vec3(2.0f, 2.0f, 2.0f));
		l2->lightColor = glm::vec3(0.5f, 0.5f, 1.0f);   // 偏蓝
		l2->range = 15.0f;
		light2->addComponent(l2);
		scene.add(light2);

		// 灯 3
		SceneObject* light3 = new SceneObject();
		LightComponent* l3 = new LightComponent();
		light3->SetPos(glm::vec3(0.0f, 5.0f, -6.0f));
		l3->lightColor = glm::vec3(1.0f, 1.0f, 0.5f);   // 偏黄
		l3->range = 15.0f;
		l3->direction = glm::vec3(0.0f, -1.0f, 0.0f);
		l3->cutoff = 0.976f;   // ≈ cos(12.5°)
		l3->cutoffOuter = 0.92f;   // ≈ cos(23°)
		l3->castShadow = true;
		light3->addComponent(l3);
		scene.add(light3);
		// PBR 对比球阵：左 4 金属右 4 塑料，roughness 从左到右递增
		{
			const char* mats[8] = {
				"mat/pbr_metal_r05.mat", "mat/pbr_metal_r25.mat",
				"mat/pbr_metal_r50.mat", "mat/pbr_metal_r75.mat",
				"mat/pbr_plastic_r05.mat", "mat/pbr_plastic_r25.mat",
				"mat/pbr_plastic_r50.mat", "mat/pbr_plastic_r75.mat",
			};
			for (int i = 0; i < 8; i++)
			{
				SceneObject* ball = new SceneObject();
				ball->SetPos(glm::vec3(-7.0f + i * 2.0f, -0.5f, -9.0f));
				ball->addComponent(new MeshRenderer(mats[i], "builtin:sphere"));
				scene.add(ball);
			}

			// PBR 专用灯：物理光强尺度 + range 恰好罩住球阵、够不着 lit 物体
			SceneObject* pbrLight = new SceneObject();
			LightComponent* pl = new LightComponent();
			pbrLight->SetPos(glm::vec3(0.0f, 20.0f, -30.0f));
			pl->lightColor = glm::vec3(3500.0f);
			pl->range = 32.5f;
			pbrLight->addComponent(pl);
			scene.add(pbrLight);

			SceneObject* plateBall = new SceneObject();
			plateBall->SetPos(glm::vec3(-8.5f, -0.5f, -9.0f));
			plateBall->addComponent(new MeshRenderer("mat/pbr_metal_plate.mat", "builtin:sphere"));
			scene.add(plateBall);
			SceneObject* tileBall = new SceneObject();
			tileBall->SetPos(glm::vec3(8.5f, -0.5f, -9.0f));
			tileBall->addComponent(new MeshRenderer("mat/pbr_floor_tiles.mat", "builtin:sphere"));
			scene.add(tileBall);

		}
	}
}

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