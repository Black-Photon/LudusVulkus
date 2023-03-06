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
	static std::vector<PhysicalDevice> get_device_list(Instance& instance, Surface& surface, std::set<std::string> required_extensions);

	PhysicalDevice(VkPhysicalDevice device, Surface& surface);

	VkPhysicalDevice get() const;
	VkPhysicalDeviceMemoryProperties get_memory_properties() const;
	uint32_t find_memory_type(uint32_t type_mask, VkMemoryPropertyFlags properties) const;
		
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

