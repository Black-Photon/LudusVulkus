#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <set>

#include "QueueFamily.h"
#include "Surface.h"

enum QueueType {
	GRAPHICS,
	COMPUTE,
	TRANSFER,
	PRESENT
};

class PhysicalDevice {
public:
	static std::vector<std::shared_ptr<PhysicalDevice>> get_device_list(VkInstance instance, Surface& surface, std::set<std::string> required_extensions);

	PhysicalDevice(VkPhysicalDevice device, Surface& surface);

	VkPhysicalDevice& get();
		
	std::vector<QueueFamily> queue_families;
	std::map<QueueType, QueueFamily> selected_family;
	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures device_features;

private:
	VkPhysicalDevice device;
	
	int find_suitability(std::set<std::string> required_extensions, Surface& surface);
	bool has_required_extension_support(std::set<std::string> required_extensions);
	std::vector<VkExtensionProperties> get_supported_extensions();
};

