#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class PhysicalDevice {
public:
	static std::vector<PhysicalDevice> get_device_list(VkInstance instance);

	PhysicalDevice(VkPhysicalDevice device);
private:
	VkPhysicalDevice device;
	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures device_features;
	
	static int get_device_suitability(PhysicalDevice device);
};

