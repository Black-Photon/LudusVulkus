#pragma once

#include <vulkan/vulkan.h>

#include "Instance.h"
#include "Window.h"

class Surface {
public:
	Surface(const Instance& instance, const Window& window);
	~Surface();

	VkSurfaceKHR get();

private:
	VkSurfaceKHR surface;
	VkInstance instance;
};

