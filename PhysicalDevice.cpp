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
		int suitability = device.find_suitability();
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

int PhysicalDevice::find_suitability() {
	const int NOT_SUITABLE = INT_MIN;
	int suitability = 0;

	// Bits 2-3 contain device type preferences
	switch (device_properties.deviceType) {
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		suitability += 3 << 2;
		break;
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		suitability += 2 << 2;
		break;
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
	case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		suitability += 1 << 2;
	}

	// Bits 0-1 contain families supported
	bool graphics_family_exists = false;
	bool compute_family_exists = false;
	bool transfer_family_exists = false;
	for (auto &queue_family : queue_families) {
		if (queue_family.supports_graphics) graphics_family_exists = true;
		if (queue_family.supports_compute) compute_family_exists = true;
		if (queue_family.supports_transfer) transfer_family_exists = true;
	}
	if (!graphics_family_exists) suitability = NOT_SUITABLE;
	if (compute_family_exists) suitability += 2 << 0;
	if (transfer_family_exists) suitability += 1 << 0;

	return suitability;
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice device) : device(device) {
	vkGetPhysicalDeviceProperties(device, &device_properties);
	vkGetPhysicalDeviceFeatures(device, &device_features);

	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

	std::vector<VkQueueFamilyProperties> vk_queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, vk_queue_families.data());

	uint32_t index = 0;
	for (auto &vk_queue_family : vk_queue_families) {
		queue_families.push_back(QueueFamily(vk_queue_family, index++));
		QueueFamily& queue_family = queue_families.back();

		if (queue_family.supports_graphics && !selected_family.contains(GRAPHICS)) selected_family.insert(std::pair(GRAPHICS, queue_family));
		if (queue_family.supports_compute && !selected_family.contains(COMPUTE)) selected_family.insert(std::pair(COMPUTE, queue_family));
		if (queue_family.supports_transfer && !selected_family.contains(TRANSFER)) selected_family.insert(std::pair(TRANSFER, queue_family));
	}
}

VkPhysicalDevice &PhysicalDevice::get() {
	return device;
}