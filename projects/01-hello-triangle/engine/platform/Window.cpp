#include "platform/Window.h"
#include "core/Log.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

Window::Window(int width, int height, const char* title)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!m_window)
	{
		LOG_ERROR("GLFW: window create failed");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		LOG_ERROR("GLAD: loader init failed");

	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
}

Window::~Window()
{
	if (m_window) glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool Window::ShouldClose() const { return glfwWindowShouldClose(m_window) != 0; }
void Window::PollEvents() { glfwPollEvents(); }
void Window::SwapBuffers() { glfwSwapBuffers(m_window); }
void Window::GetFramebufferSize(int& w, int& h) const { glfwGetFramebufferSize(m_window, &w, &h); }

void Window::SetResizeCallback(std::function<void(int, int)> cb)
{
	m_resizeCb = cb;
	int fbw, fbh;
	glfwGetFramebufferSize(m_window, &fbw, &fbh);
	if (m_resizeCb) m_resizeCb(fbw, fbh);
}

void Window::FramebufferSizeCallback(GLFWwindow* w, int width, int height)
{
	auto self = (Window*)glfwGetWindowUserPointer(w);
	if (self && self->m_resizeCb) self->m_resizeCb(width, height);
}