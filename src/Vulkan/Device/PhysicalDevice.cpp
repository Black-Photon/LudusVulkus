#include "PhysicalDevice.h"

#include <stdexcept>
#include <map>
#include <ranges>

#include "Logger.h"
#include "SwapChainDetails.h"
#include "Helper.h"

std::vector<PhysicalDevice> PhysicalDevice::get_device_list(Instance& instance, Surface& surface, std::set<std::string> required_extensions) {
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(instance.get(), &device_count, nullptr);
	std::vector<VkPhysicalDevice> supported_devices(device_count);
	vkEnumeratePhysicalDevices(instance.get(), &device_count, supported_devices.data());

	if (device_count == 0) {
		throw std::runtime_error("Failed to find GPU with Vulkan support");
	}

	std::multimap<int, PhysicalDevice> device_options;
	for (const auto& vk_device : supported_devices) {
		PhysicalDevice device(vk_device, surface);
		int suitability = device.find_suitability(required_extensions, surface);
		device_options.insert(std::make_pair(suitability, device));
	}

	Logger::log("Starting device registration (priority TOP->BOTTOM)", Logger::INFO);

	std::vector<PhysicalDevice> devices;
	for (const auto& device : std::ranges::views::reverse(device_options)) {
		if (device.first > 0) {
			devices.push_back(device.second);

			Logger::log("\tDevice with name \"" + std::string(device.second.device_properties.deviceName) + "\" registered");
			std::map<QueueType, QueueFamily> selected_family = device.second.selected_family;
			auto has_support = [selected_family](QueueType queue_type) {
				return std::string(selected_family.find(queue_type) != selected_family.end() ? "true" : "false");
			};
			Logger::log("\t - Graphics: " + has_support(GRAPHICS), Logger::VERBOSE);
			Logger::log("\t - Compute:  " + has_support(COMPUTE), Logger::VERBOSE);
			Logger::log("\t - Transfer: " + has_support(TRANSFER), Logger::VERBOSE);
			Logger::log("\t - Present:  " + has_support(PRESENT), Logger::VERBOSE);
			Logger::log("\tSuitability is " + std::to_string(device.first), Logger::VERBOSE);
		}
	}

	if (devices.size() == 0) {
		throw std::runtime_error("None of the provided GPUs met the minimum requirements needed");
	}

	return devices;
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice device, Surface &surface) : device(device) {
	vkGetPhysicalDeviceProperties(device, &device_properties);
	vkGetPhysicalDeviceFeatures(device, &device_features);

	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

	std::vector<VkQueueFamilyProperties> vk_queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, vk_queue_families.data());

	Logger::log("Enumerating queue families for device " + std::string(device_properties.deviceName), Logger::VERBOSE);

	uint32_t index = 0;
	for (auto &vk_queue_family : vk_queue_families) {
		queue_families.push_back(QueueFamily(vk_queue_family, index++, *this, surface));
		QueueFamily& queue_family = queue_families.back();

		// TODO Don't just use first queue family that support each capability
		if (queue_family.supports_graphics && !selected_family.contains(GRAPHICS)) selected_family.insert(std::pair(GRAPHICS, queue_family));
		if (queue_family.supports_compute && !selected_family.contains(COMPUTE)) selected_family.insert(std::pair(COMPUTE, queue_family));
		if (queue_family.supports_transfer && !selected_family.contains(TRANSFER)) selected_family.insert(std::pair(TRANSFER, queue_family));
		if (queue_family.supports_present && !selected_family.contains(PRESENT)) selected_family.insert(std::pair(PRESENT, queue_family));

		Logger::log("Found queue family:", Logger::VERBOSE);
		Logger::log("\t - Graphics: " + bool_str(selected_family.contains(GRAPHICS)), Logger::VERBOSE);
		Logger::log("\t - Compute:  " + bool_str(selected_family.contains(COMPUTE)), Logger::VERBOSE);
		Logger::log("\t - Transfer: " + bool_str(selected_family.contains(TRANSFER)), Logger::VERBOSE);
		Logger::log("\t - Present:  " + bool_str(selected_family.contains(PRESENT)), Logger::VERBOSE);
	}
}

VkPhysicalDevice PhysicalDevice::get() const {
	return device;
}

VkPhysicalDeviceMemoryProperties PhysicalDevice::get_memory_properties() const {
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(device, &memory_properties);
	return memory_properties;
}

uint32_t PhysicalDevice::find_memory_type(uint32_t type_mask, VkMemoryPropertyFlags properties) const {
	VkPhysicalDeviceMemoryProperties memory_properties = get_memory_properties();

	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
		if (type_mask & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("No memory type matches mask");
}

int PhysicalDevice::find_suitability(std::set<std::string> required_extensions, Surface& surface) {
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
	bool present_family_exists = false;
	for (auto& queue_family : queue_families) {
		if (queue_family.supports_graphics) graphics_family_exists = true;
		if (queue_family.supports_compute) compute_family_exists = true;
		if (queue_family.supports_transfer) transfer_family_exists = true;
		if (queue_family.supports_present) present_family_exists = true;
	}
	if (!graphics_family_exists || !present_family_exists) suitability = NOT_SUITABLE;
	if (compute_family_exists) suitability += 2 << 0;
	if (transfer_family_exists) suitability += 1 << 0;

	if(!has_required_extension_support(required_extensions)) suitability = NOT_SUITABLE;
	else {
		// Slight inefficiency as we need to query the details again when creating the swapchain
		SwapChainDetails swap_chain_details(*this, surface);
		bool swap_chain_sufficient = !swap_chain_details.formats.empty() && !swap_chain_details.present_modes.empty();
		if (!swap_chain_sufficient) suitability = NOT_SUITABLE;
	}

	return suitability;
}

bool PhysicalDevice::has_required_extension_support(std::set<std::string> required_extensions) {
	std::vector<VkExtensionProperties> available_extensions = get_supported_extensions();

	for (auto& extension : available_extensions) {
		required_extensions.erase(extension.extensionName);
	}

	return required_extensions.empty();
}

std::vector<VkExtensionProperties> PhysicalDevice::get_supported_extensions() {
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> available_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());
	return available_extensions;
}

VkFormat PhysicalDevice::first_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("None of the formats provided were supported");
}