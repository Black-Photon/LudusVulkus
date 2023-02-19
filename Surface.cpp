#include "Surface.h"

#include <stdexcept>

#include "Logger.h"

Surface::Surface(const Instance &instance, const Window &window) : instance(instance.get()) {
    if (glfwCreateWindowSurface(instance.get(), window.get(), nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
}

Surface::~Surface() {
    Logger::log("Freeing Surface", Logger::VERBOSE);
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

VkSurfaceKHR Surface::get() {
    return surface;
}