#include "Window.h"

#include <GLFW/glfw3.h>

Window::Window(std::string name, int width, int height, bool resizable) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
	window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
}

Window::~Window() {
	glfwDestroyWindow(window);
}

bool Window::should_close() {
	return glfwWindowShouldClose(window);
}

GLFWwindow* Window::get() {
	return window;
}