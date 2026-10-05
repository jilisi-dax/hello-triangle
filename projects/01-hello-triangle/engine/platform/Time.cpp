#include "platform/Time.h"
#include <GLFW/glfw3.h>

float Time::Now() { return (float)glfwGetTime(); }

