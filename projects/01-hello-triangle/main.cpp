#include "pch.h"
#include "main.h"
#include "Shader.h"
#include "camera.h"
#include "keyBoard.h"
#include "role.h"
#include "Scene.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
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
		printf("GLFW faild");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("falad");
		return -1;
	}

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	SceneObject* cameraObj = new SceneObject();
	CameraComponent* cam = new CameraComponent();
	cameraObj->addComponent(cam);
	Scene scene;
	scene.add(new cubeModel());
	scene.add(new modelObj());
	scene.add(new ground());
	scene.add(cameraObj);
	scene.setMainCamera(cam);

	{
		SceneObject* lightObj = new SceneObject();
		LightComponent* light = new LightComponent();
		lightObj->SetPos(glm::vec3(-2.0f, -2.0f, -5.0f));
		light->lightColor = glm::vec3(1.0f);
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
		processInput(window, *cam, deltaTime);


		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		scene.render();

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glfwTerminate();
	return 0;





}