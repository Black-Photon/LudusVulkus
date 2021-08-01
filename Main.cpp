#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <GLFW/glfw3.h>

#include "LudusVulkus.h"
#include "Logger.h"
#include "TestApplication.h"

int main() {
    // Load GLFW for future use
    glfwInit();

    try {
        auto app = std::make_unique<TestApplication>();

        LudusVulkus ludus_vulkus(std::move(app));
        ludus_vulkus.run();
    } catch (const std::exception& e) {
        Logger::log(e.what(), Logger::ERROR);
        return EXIT_FAILURE;
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}