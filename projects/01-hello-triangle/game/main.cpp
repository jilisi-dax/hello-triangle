#include "core/pch.h"
#include "Game.h"
#include "scene/components/CameraComponent.h"
#include "scene/Scene.h"
#include "render/ResourceLib.h"
#include "render/Renderer.h"
#include "platform/Input.h" 
#include "platform/ActionMap.h"
#include "platform/Window.h"
#include "platform/Time.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

Renderer* renderer = nullptr;

int main()
{
	SetConsoleOutputCP(CP_UTF8);
	Window window(800, 800, "画个三角形");
	renderer = new Renderer();

	init(scene, window.NativeHandle());
	const float FIXED_DT = 1.0f;
	float accumulator = 0.0f;
	float lastFrame = Time::Now();
	int fbw, fbh;
	while (!window.ShouldClose())
	{
		float frameTime = Time::Now() - lastFrame;
		lastFrame = Time::Now();
		if (frameTime > 0.25f) frameTime = 0.25f;   // 防死亡螺旋：最多补 15 步

		Input::NewFrame();
		window.PollEvents();
		processInput(*cam, frameTime);

		accumulator += frameTime;
		while (accumulator >= FIXED_DT)
		{
			scene.update(FIXED_DT);
			accumulator -= FIXED_DT;
		}

		if (ActionMap::WasPressed(GameAction::EffectGray))    renderer->ToggleEffectMode(1);
		if (ActionMap::WasPressed(GameAction::EffectInvert))  renderer->ToggleEffectMode(2);
		if (ActionMap::WasPressed(GameAction::EffectSharpen)) renderer->ToggleEffectMode(3);
		if (ActionMap::WasPressed(GameAction::EffectBloomDebug)) renderer->ToggleEffectMode(4);
		if (ActionMap::WasPressed(GameAction::EffectIrradianceDebug)) renderer->CycleEnvDebug();

		window.GetFramebufferSize(fbw, fbh);
		renderer->RenderFrame(scene, fbw, fbh);
		window.SwapBuffers();
	}

	scene.clear();
	ResourceLib::Shutdown();
	delete renderer;
	return 0;
}

