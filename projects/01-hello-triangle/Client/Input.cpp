#include "Input.h"

GLFWwindow* Input::s_window = nullptr;
std::array<bool, GLFW_KEY_LAST + 1> Input::s_down{};
std::array<bool, GLFW_KEY_LAST + 1> Input::s_lastDown{};

bool Input::s_firstMouse = true;
double Input::s_lastX = 0.0;
double Input::s_lastY = 0.0;
glm::vec2 Input::s_accDelta(0.0f);
float Input::s_accScroll = 0.0f;
glm::vec2 Input::s_frameDelta(0.0f);
float Input::s_frameScroll = 0.0f;

void Input::Init(GLFWwindow* window)
{
    s_window = window;
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
}

void Input::NewFrame()
{
    s_frameDelta = s_accDelta;
    s_accDelta = glm::vec2(0.0f);
    s_frameScroll = s_accScroll;
    s_accScroll = 0.0f;

    s_lastDown = s_down;
}

bool Input::IsDown(int key) { return s_down[key]; }
bool Input::WasPressed(int key) { return s_down[key] && !s_lastDown[key]; }
bool Input::IsMouseButtonDown(int button) { return glfwGetMouseButton(s_window, button) == GLFW_PRESS; }
glm::vec2 Input::GetMouseDelta() { return s_frameDelta; }
float Input::GetScrollDelta() { return s_frameScroll; }
void Input::RequestExit() { glfwSetWindowShouldClose(s_window, GLFW_TRUE); }

void Input::KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods)
{
    if (key < 0 || key > GLFW_KEY_LAST) return;
    s_down[key] = (action != GLFW_RELEASE); 
}

void Input::CursorPosCallback(GLFWwindow* w, double xpos, double ypos)
{
    if (s_firstMouse)
    {
        s_lastX = xpos;
        s_lastY = ypos;
        s_firstMouse = false;
        return;
    }
    s_accDelta += glm::vec2(float(xpos - s_lastX), float(ypos - s_lastY));
    s_lastX = xpos;
    s_lastY = ypos;

}
void Input::ScrollCallback(GLFWwindow* w, double xoff, double yoff)
{
    s_accScroll += (float)yoff; 
}