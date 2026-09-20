#pragma once
#include "pch.h"

class Input
{
public:
	static void Init(GLFWwindow* window);
	static void NewFrame();

	static bool IsDown(int key);
	static bool WasPressed(int key);
	static bool IsMouseButtonDown(int button);
	static glm::vec2 GetMouseDelta();
	static float GetScrollDelta();
	static void RequestExit();

private:
	static void KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
	static void CursorPosCallback(GLFWwindow* w, double xpos, double ypos);
	static void ScrollCallback(GLFWwindow* w, double xoff, double yoff);

	static GLFWwindow* s_window;
	static std::array<bool, GLFW_KEY_LAST + 1> s_down;
	static std::array<bool, GLFW_KEY_LAST + 1> s_lastDown;
	static bool s_firstMouse;
	static double s_lastX;
	static double s_lastY;
	static glm::vec2 s_accDelta;
	static float s_accScroll;
	static glm::vec2 s_frameDelta;
	static float s_frameScroll;

};