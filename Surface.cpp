#include "Surface.h"

#include <stdexcept>

Surface::Surface(Instance &instance, Window &window) : instance(instance.get()) {
    if (glfwCreateWindowSurface(instance.get(), window.get(), nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
}

Surface::~Surface() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

VkSurfaceKHR Surface::get() {
    return surface;
}