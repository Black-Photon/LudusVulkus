#include "LudusVulkus.h"

#include <GLFW/glfw3.h>

#include "Logger.h"
#include "Window.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Surface.h"

LudusVulkus::LudusVulkus(std::unique_ptr<Application> app) :
    app{ std::move(app) }
{
    // Setup any constants
    settings = std::make_shared<Settings>();
    setup_settings();

    // Initialise Window
    window = std::make_shared<Window>("Ludus Vulkus", 800, 600, false);

    std::vector<std::string> validation_layers = select_validation_layers();

    // Initialise Instance
    instance = std::make_shared<Instance>(
        this->app->name, this->app->version,    // App details
        "Ludus Vulkus", Version{ 1, 0, 0 },     // Engine details
        VK_API_VERSION_1_0,                     // Vulkan version
        settings,                               // Reference to settings
        prepare_extensions(),                   // Extensions to load
        validation_layers                       // Validation layers to load
    );

    Surface surface(*instance, *window);

    std::vector<PhysicalDevice> physical_devices = PhysicalDevice::get_device_list(instance->get(), surface);
    std::vector<std::unique_ptr<Device>> devices;
    for (auto& physical_device : physical_devices) {
        std::unique_ptr<Device> device = std::make_unique<Device>(physical_device, physical_device.selected_family.at(GRAPHICS), *settings, validation_layers);
        devices.push_back(std::move(device));
    }
}

void LudusVulkus::setup_settings() {
#ifdef NDEBUG
    bool debug = false;
#else
    bool debug = true;
#endif
    settings->use_validation_layers = debug;
    settings->close_on_non_fatal = false;

    settings->layer_verbose_enable = false;
    settings->layer_info_enable = false;
    settings->layer_warn_enable = true;
    settings->layer_error_enable = true;
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

    // Find extensions needed by glfw
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<std::string> extensions = std::vector<std::string>(glfwExtensions, glfwExtensions + glfwExtensionCount);

    // Add extensions for validation layer if needed
    if (settings->use_validation_layers) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

std::vector<std::string> LudusVulkus::select_validation_layers() {
    std::vector<std::string> layers = std::vector<std::string>();
    layers.push_back("VK_LAYER_KHRONOS_validation");
    return layers;
}