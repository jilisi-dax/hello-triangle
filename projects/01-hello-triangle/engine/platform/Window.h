#pragma
#include "core/pch.h"

struct GLFWwindow;

class Window
{
public:
	Window(int width, int height, const char* title);
	~Window();

	bool ShouldClose() const;
	void PollEvents();
	void SwapBuffers();
	void GetFramebufferSize(int& w, int& h) const;
	GLFWwindow* NativeHandle() const { return m_window; }
	void SetResizeCallback(std::function<void(int, int)> cb);

private:
	static void FramebufferSizeCallback(GLFWwindow* w, int width, int height);
	GLFWwindow* m_window = nullptr;
	std::function<void(int, int)> m_resizeCb;
};
