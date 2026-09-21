#include "pch.h"
#include "main.h"
#include "camera.h"
#include "keyBoard.h"
#include "role.h"
#include "Scene.h"
#include "Input.h" 
#include "ResourceLib.h"
#include "ActionMap.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

CameraComponent* g_mainCamera = nullptr;
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
	Input::Init(window);
	ActionMap::Init();
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOG_ERROR("faild");
		return -1;
	}

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	SceneObject* cameraObj = new SceneObject();
	CameraComponent* cam = new CameraComponent();
	cameraObj->addComponent(cam);
	Scene scene;
	cubeModel* cube = new cubeModel();
	scene.add(cube);
	scene.add(new metalCube());
	scene.add(new modelObj());
	scene.add(new ground());
	scene.add(cameraObj);
	scene.setMainCamera(cam);
	g_mainCamera = cam;
	int fbw, fbh;
	glfwGetFramebufferSize(window, &fbw, &fbh);
	cam->setAspect((float)fbw, (float)fbh);

	{
		SceneObject* lightObj = new SceneObject();
		LightComponent* light = new LightComponent();
		lightObj->SetPos(glm::vec3(0.0f, 0.5f, 8.0f));
		light->lightColor = glm::vec3(1.0f);
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
		light3->SetPos(glm::vec3(0.0f, 5.0f, 0.0f));
		l3->lightColor = glm::vec3(1.0f, 1.0f, 0.5f);   // 偏黄
		l3->range = 10.0f;
		light3->addComponent(l3);
		scene.add(light3);

	}

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		Input::NewFrame();
		processInput(*cam, deltaTime);


		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		scene.update(deltaTime);
		scene.render();



		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	
	scene.clear(); 
	ResourceLib::Shutdown();
	glfwTerminate();
	return 0;





}