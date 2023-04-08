#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <GLFW/glfw3.h>

#include "LudusVulkus.h"
#include "Logger.h"
#include "TriangleEngine.h"
#include "Vulkus3D.h"

int main() {
    // Load GLFW for future use
    glfwInit();

    try {
        LudusVulkus<TriangleEngine> ludus_vulkus;
        ludus_vulkus.run();
    } catch (const std::exception& e) {
        Logger::log(e.what(), Logger::FATAL);
        return EXIT_FAILURE;
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}