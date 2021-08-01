#pragma once

#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
	Window(std::string name, int width = 800, int height = 600, bool resizable = true);
	~Window();
	bool should_close();

private:
	GLFWwindow* window;
};

