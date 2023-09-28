#pragma once

#include "resource.h"

#include <memory>

#include "Application.h"
#include "Window.h"
#include "Logger.h"

template <class App>
class LudusVulkus {
public:
    void run() {
        Logger::log("Starting application");

        // Setup any constants
        setup_settings();

        std::set<std::string> validation_layers = select_validation_layers();
        std::set<std::string> device_extensions = prepare_device_extensions();

        // Initialise Instance
        Instance instance(
            App::name, App::version,                          // App details
            "Ludus Vulkus", Version{ 1, 0, 0 },     // Engine details
            VK_API_VERSION_1_0,                     // Vulkan version
            settings,                               // Reference to settings
            prepare_extensions(),                   // Extensions to load
            validation_layers                       // Validation layers to load
        );

        // Initialise Window
        Window window(App::name, 800, 600);

        Surface surface(instance, window);

        std::vector<PhysicalDevice> physical_devices = PhysicalDevice::get_device_list(instance, surface, device_extensions);
        // Physical devices are ordered such that the most suitable for graphics are listed first
        // As such we'll just use the first device we find for now
        PhysicalDevice& physical_device = physical_devices.at(0);
        Device device(physical_device, physical_device.selected_family.at(GRAPHICS), settings, device_extensions, validation_layers);

        App app(instance, device, window, surface, settings);

        glfwSetWindowUserPointer(window.get(), reinterpret_cast<void*>(&app));

        app.prepare();

        while (!window.should_close()) {
            glfwPollEvents();

            try {
                app.update();
            } catch (SwapChainOutdated& e) {
                Logger::log("Recreating swap chain", Logger::VERBOSE);
                app.recreate_swapchain();
            }
        }
        app.on_close();
    }

private:
    Settings settings;

    void setup_settings() {
#ifdef NDEBUG
        bool debug = false;
#else
        bool debug = true;
#endif
        settings.use_validation_layers = debug;
        settings.close_on_non_fatal = false;

        settings.layer_verbose_enable = false;
        settings.layer_info_enable = false;
        settings.layer_warn_enable = true;
        settings.layer_error_enable = true;

        settings.is_mobile = is_mobile();
    }

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

    bool is_mobile() {
        // From https://stackoverflow.com/a/5920028
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    // For now assume that all windows devices aren't on mobile (sorry Microsoft)
        return false;
#elif __APPLE__
#if TARGET_IPHONE_SIMULATOR
    // iOS, tvOS, or watchOS Simulator
        return true;
#elif TARGET_OS_MACCATALYST
    // Mac's Catalyst (ports iOS API into Mac, like UIKit).
        return false;
#elif TARGET_OS_IPHONE
    // iOS, tvOS, or watchOS device
        return true;
#elif TARGET_OS_MAC
    // Other kinds of Apple platforms
        return false;
#else
        Logger::log("Unknown Apple Device: Assuming non-mobile", Logger::NONFATAL);
        return false;
#endif
#elif __ANDROID__
        return true;
#elif __linux__
        return false;
#elif __unix__
        return false;
#elif defined(_POSIX_VERSION)
        return false;
#else
        Logger::log("Unknown compiler target: Assuming non-mobile", Logger::NONFATAL);
        return false;
#endif
    }

    std::set<std::string> prepare_extensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        // Find extensions needed by glfw
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::set<std::string> extensions = std::set<std::string>(glfwExtensions, glfwExtensions + glfwExtensionCount);

        // Add extensions for validation layer if needed
        if (settings.use_validation_layers) extensions.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    std::set<std::string> prepare_device_extensions() {
        std::set<std::string> layers = std::set<std::string>();
        layers.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        return layers;
    }

    std::set<std::string> select_validation_layers() {
        std::set<std::string> layers = std::set<std::string>();
        layers.insert("VK_LAYER_KHRONOS_validation");
        //layers.insert("VK_LAYER_LUNARG_api_dump");
        return layers;
    }
};