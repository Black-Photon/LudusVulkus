#include "Instance.h"

#include <stdexcept>
#include <GLFW/glfw3.h>
#include <cassert>

#include "Logger.h"

Instance::Instance(
    std::string app_name, Version app_version,
    std::string engine_name, Version engine_version,
    int vulkan_version, std::vector<std::string> required_extensions,
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
        for (auto available_extension : available_extensions) {
            if (required_extension == available_extension.extensionName) {
                found = true;
                break;
            }
        }
        if (!found) throw std::runtime_error("Could not load extension " + required_extension + ".");
    }

    // Copy extension list into new structure
    char** c_extensions = new char* [required_extensions.size()];
    for (int index = 0; index < required_extensions.size(); index++) {
        c_extensions[index] = const_cast<char*>(required_extensions[index].c_str());
        Logger::log("Loading extension " + required_extensions[index]);
    }


    // --- VALIDATION LAYERS --- //

    ValidationLayers layers_ref;
    for (auto layer : required_layers) {
        if (!layers_ref.is_layer_available(layer)) {
            throw std::runtime_error("Required layer " + layer + " is not available.");
        }
    }

    // Copy validation layer list into new structure
    char** c_layers = new char* [required_layers.size()];
    for (int index = 0; index < required_layers.size(); index++) {
        c_layers[index] = const_cast<char*>(required_layers[index].c_str());
        Logger::log("Loading validation layer " + required_layers[index]);
    }

    create_info.enabledExtensionCount = required_extensions.size();
    create_info.ppEnabledExtensionNames = c_extensions;
    create_info.enabledLayerCount = required_layers.size();
    create_info.ppEnabledLayerNames = c_layers;

    VkInstance i;
    VkResult result = vkCreateInstance(&create_info, nullptr, &i);
    if (result != VK_SUCCESS) throw std::runtime_error("Failed to create instance.");

    instance = std::make_unique<VkInstance>(i);

    // Delete extension list
    delete[] c_extensions;
}

Instance::~Instance() {
    vkDestroyInstance(*instance, nullptr);
}