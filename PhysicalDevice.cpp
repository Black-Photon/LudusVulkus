#include "PhysicalDevice.h"

#include <stdexcept>
#include <map>

#include "Logger.h"

std::vector<PhysicalDevice> PhysicalDevice::get_device_list(VkInstance instance) {
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
	std::vector<VkPhysicalDevice> supported_devices(device_count);
	vkEnumeratePhysicalDevices(instance, &device_count, supported_devices.data());

	if (device_count == 0) {
		throw std::runtime_error("Failed to find GPU with Vulkan support");
	}

	std::multimap<int, PhysicalDevice> device_options;
	for (const auto& vk_device : supported_devices) {
		PhysicalDevice device(vk_device);
		int suitability = get_device_suitability(device);
		device_options.insert(std::make_pair(suitability, device));
	}

	Logger::log("Starting device registration (priority TOP->BOTTOM)", Logger::INFO);

	std::vector<PhysicalDevice> devices;
	for (const auto& device : device_options) {
		if (device.first > 0) {
			devices.push_back(device.second);
			Logger::log("\tDevice with name \"" + std::string(device.second.device_properties.deviceName) + "\" registered", Logger::INFO);
		}
	}

	if (devices.size() == 0) {
		throw std::runtime_error("None of the provided GPUs met the minimum requirements needed");
	}

	return devices;
}

int PhysicalDevice::get_device_suitability(PhysicalDevice device) {
	VkPhysicalDeviceProperties properties = device.device_properties;
	VkPhysicalDeviceFeatures features = device.device_features;

	int suitability = 0;

	// For now just rank by the GPU type
	switch (properties.deviceType) {
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		suitability += 3;
		break;
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		suitability += 2;
		break;
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
	case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		suitability += 1;
	}

	return suitability;
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice device) : device(device) {
	vkGetPhysicalDeviceProperties(device, &device_properties);
	vkGetPhysicalDeviceFeatures(device, &device_features);
}