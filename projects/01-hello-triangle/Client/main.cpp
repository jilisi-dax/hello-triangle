#include "pch.h"
#include "main.h"
#include "camera.h"
#include "keyBoard.h"
#include "role.h"
#include "Scene.h"
#include "Input.h" 
#include "ResourceLib.h"
#include "ActionMap.h"
#include "Renderer.h"
#include "renderComponent.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

CameraComponent* g_mainCamera = nullptr;
Scene scene;
CameraComponent* cam = new CameraComponent();
Renderer* renderer = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	if (g_mainCamera)
		g_mainCamera->setAspect((float)width, (float)height);
};


int main()
{
	SetConsoleOutputCP(CP_UTF8);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "画个三角形", NULL,	NULL);
	if (window == NULL)
	{
		LOG_ERROR("GLFW faild");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOG_ERROR("faild");
		return -1;
	}
	renderer = new Renderer();

	init(scene, window);
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	int fbw, fbh;
	glfwGetFramebufferSize(window, &fbw, &fbh);
	framebuffer_size_callback(window, fbw, fbh);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		Input::NewFrame();
		glfwPollEvents();
		processInput(*cam, deltaTime);

		scene.update(deltaTime);
		if (ActionMap::WasPressed(GameAction::EffectGray))    renderer->ToggleEffectMode(1);
		if (ActionMap::WasPressed(GameAction::EffectInvert))  renderer->ToggleEffectMode(2);
		if (ActionMap::WasPressed(GameAction::EffectSharpen)) renderer->ToggleEffectMode(3);
		if (ActionMap::WasPressed(GameAction::EffectBloomDebug)) renderer->ToggleEffectMode(4);
		if (ActionMap::WasPressed(GameAction::EffectIrradianceDebug)) renderer->CycleEnvDebug();

		glfwGetFramebufferSize(window, &fbw, &fbh);
		renderer->RenderFrame(scene, fbw, fbh);


		glfwSwapBuffers(window);

	}
	
	scene.clear(); 
	ResourceLib::Shutdown();
	delete renderer;
	glfwTerminate();
	return 0;





}


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