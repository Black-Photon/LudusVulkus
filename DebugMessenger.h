#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <optional>

#include "Settings.h"

class DebugMessenger
{
public:
	DebugMessenger(Settings &settings);
	~DebugMessenger();

	void create_messenger(VkInstance instance);

	static VKAPI_ATTR VkBool32 VKAPI_CALL layer_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* data);

	VkDebugUtilsMessengerCreateInfoEXT create_info;

private:
	std::optional<VkInstance> instance;
	Settings settings;
	VkDebugUtilsMessengerEXT debug_messenger;
};

