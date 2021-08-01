#include "LudusVulkus.h"

#include <GLFW/glfw3.h>

#include "Logger.h"
#include "Window.h"
#include "Instance.h"

LudusVulkus::LudusVulkus(std::unique_ptr<Application> app) :
    app{ std::move(app) }
{
    // Initialise Window
    window = std::make_unique<Window>("Ludus Vulkus", 800, 600, false);

    // Initialise Instance
    instance = std::make_unique<Instance>(
        this->app->name, this->app->version,    // App details
        "Ludus Vulkus", Version{ 1, 0, 0 },     // Engine details
        VK_API_VERSION_1_0,                     // Vulkan version
        prepare_extensions(),                   // Extensions to load
        select_validation_layers()              // Validation layers to load
    );
}

void LudusVulkus::run() {
    Logger::log("Starting application");

    app->prepare();
    
    run_loop();
}

void LudusVulkus::run_loop() {
    while (!window->should_close()) {
        glfwPollEvents();
    }
}


std::vector<std::string> LudusVulkus::prepare_extensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    return std::vector<std::string>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

std::vector<std::string> LudusVulkus::select_validation_layers() {
    std::vector<std::string> layers = std::vector<std::string>();
    layers.push_back("VK_LAYER_KHRONOS_validation");
    return layers;
}