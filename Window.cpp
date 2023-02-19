#include "Window.h"

#include <GLFW/glfw3.h>

#include "Logger.h"

Window::Window(std::string name, int width, int height, bool resizable) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
	window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
}

Window::~Window() {
	Logger::log("Freeing Window", Logger::VERBOSE);
	glfwDestroyWindow(window);
}

bool Window::should_close() {
	return glfwWindowShouldClose(window);
}

GLFWwindow* Window::get() const {
	return window;
}