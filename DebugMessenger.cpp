#include "DebugMessenger.h"

#include <stdexcept>

#include "Logger.h"

DebugMessenger::DebugMessenger(std::shared_ptr<Settings> settings) : settings(settings) {
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = 0;
    if (settings->layer_verbose_enable) create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    if (settings->layer_info_enable) create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    if (settings->layer_warn_enable) create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    if (settings->layer_error_enable) create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = layer_callback;
    create_info.pUserData = settings.get();
    create_info.pNext = nullptr;
    create_info.flags = 0;
}

DebugMessenger::~DebugMessenger() {
    if (instance.has_value()) vkDestroyDebugUtilsMessengerEXT(instance.value(), debug_messenger, nullptr);
}

void DebugMessenger::create_messenger(VkInstance instance) {
    this->instance = instance;

    if (vkCreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS) {
        throw std::runtime_error("Could not setup the debug callbacks");
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::layer_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* data) {

    Settings* settings = static_cast<Settings*>(data);

    Logger::LogLevel level;
    switch (message_severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        level = Logger::VERBOSE;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        level = Logger::INFO;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        level = Logger::WARN;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        // Always stop if breaks the spec
        if (message_type == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
            level = Logger::FATAL;
        }
        else {
            level = Logger::NONFATAL;
        }
        break;
    default:
        throw std::runtime_error("Unknown message severity level");
    }

    Logger::log(std::string(callback_data->pMessage), level);
    if (level == Logger::NONFATAL) Logger::check_except_non_fatal(*settings);

    return VK_FALSE;
}