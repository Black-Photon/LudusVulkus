#include "Instance.h"

#include <stdexcept>
#include <GLFW/glfw3.h>
#include <cassert>
#include <optional>

#include "Logger.h"

Instance::Instance(
    std::string app_name, Version app_version,
    std::string engine_name, Version engine_version,
    int vulkan_version, std::shared_ptr<Settings> settings,
    std::vector<std::string> required_extensions,
    std::vector<std::string> required_layers
) {
	VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = app_name.c_str();
    app_info.applicationVersion = VK_MAKE_VERSION(app_version.major, app_version.minor, app_version.rev);
    app_info.pEngineName = engine_name.c_str();
    app_info.engineVersion = VK_MAKE_VERSION(engine_version.major, engine_version.minor, engine_version.rev);
    app_info.apiVersion = vulkan_version;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;


    // --- EXTENSIONS --- //

    assert(required_extensions.size() < MAX_NO_EXTENSIONS);

    // Find the list of available extensions
    uint32_t available_extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, nullptr);
    std::vector<VkExtensionProperties> available_extensions(available_extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, available_extensions.data());

    // Make sure all required extensions are available
    for (auto required_extension : required_extensions) {
        bool found = false;
        for (auto &available_extension : available_extensions) {
            if (required_extension == available_extension.extensionName) {
                found = true;
                break;
            }
        }
        if (!found) throw std::runtime_error("Could not load extension " + required_extension + ".");
    }

    // Copy extension list into new structure
    const char** c_extensions = new const char* [required_extensions.size()];
    for (int index = 0; index < required_extensions.size(); index++) {
        c_extensions[index] = required_extensions[index].c_str();
        Logger::log("Loading extension " + required_extensions[index]);
    }


    // --- VALIDATION LAYERS --- //
    if (settings->use_validation_layers) {
        ValidationLayers layers_ref;
        for (auto layer : required_layers) {
            if (!layers_ref.is_layer_available(layer)) {
                throw std::runtime_error("Required layer " + layer + " is not available.");
            }
        }

        // Copy validation layer list into new structure
        const char** c_layers = new const char* [required_layers.size()];
        for (int index = 0; index < required_layers.size(); index++) {
            c_layers[index] = required_layers[index].c_str();
            Logger::log("Loading validation layer " + required_layers[index]);
        }

        debug_messenger = DebugMessenger(settings);

        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_messenger.value().create_info;
        create_info.enabledLayerCount = required_layers.size();
        create_info.ppEnabledLayerNames = c_layers;
    } else {
        create_info.pNext = nullptr;
        create_info.enabledLayerCount = 0;
    }

    create_info.enabledExtensionCount = required_extensions.size();
    create_info.ppEnabledExtensionNames = c_extensions;

    VkResult result = vkCreateInstance(&create_info, nullptr, &instance);
    if (result != VK_SUCCESS) throw std::runtime_error("Failed to create instance.");

    if (debug_messenger.has_value()) debug_messenger.value().create_messenger(instance);

    // Delete extension list
    delete[] c_extensions;
}

Instance::~Instance() {
    if (debug_messenger.has_value()) debug_messenger.reset();
    vkDestroyInstance(instance, nullptr);
}

VkInstance Instance::get_vk_instance() {
    return instance;
}