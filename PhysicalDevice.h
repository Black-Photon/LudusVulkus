#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>

#include "QueueFamily.h"

enum QueueType {
	GRAPHICS,
	COMPUTE,
	TRANSFER
};

class PhysicalDevice {
public:
	static std::vector<PhysicalDevice> get_device_list(VkInstance instance);

	PhysicalDevice(VkPhysicalDevice device);

	VkPhysicalDevice& get();
		
	std::vector<QueueFamily> queue_families;
	std::map<QueueType, QueueFamily> selected_family;
	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures device_features;

private:
	VkPhysicalDevice device;
	
	int find_suitability();
};

