#include "PhysicalDevice.h"

#include <stdexcept>
#include <map>

#include "Logger.h"
#include "SwapChainDetails.h"

std::vector<std::shared_ptr<PhysicalDevice>> PhysicalDevice::get_device_list(VkInstance instance, Surface& surface, std::set<std::string> required_extensions) {
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
	std::vector<VkPhysicalDevice> supported_devices(device_count);
	vkEnumeratePhysicalDevices(instance, &device_count, supported_devices.data());

	if (device_count == 0) {
		throw std::runtime_error("Failed to find GPU with Vulkan support");
	}

	std::multimap<int, std::shared_ptr<PhysicalDevice>> device_options;
	for (const auto& vk_device : supported_devices) {
		std::shared_ptr<PhysicalDevice> device = std::make_shared<PhysicalDevice>(vk_device, surface);
		int suitability = device->find_suitability(required_extensions, surface);
		device_options.insert(std::make_pair(suitability, device));
	}

	Logger::log("Starting device registration (priority TOP->BOTTOM)", Logger::INFO);

	std::vector<std::shared_ptr<PhysicalDevice>> devices;
	for (const auto& device : device_options) {
		if (device.first > 0) {
			devices.push_back(device.second);

			Logger::log("\tDevice with name \"" + std::string(device.second->device_properties.deviceName) + "\" registered");
			std::map<QueueType, QueueFamily> selected_family = device.second->selected_family;
			auto has_support = [selected_family](QueueType queue_type) {
				return std::string(selected_family.find(queue_type) != selected_family.end() ? "true" : "false");
			};
			Logger::log("\t - Graphics: " + has_support(GRAPHICS), Logger::VERBOSE);
			Logger::log("\t - Compute:  " + has_support(COMPUTE), Logger::VERBOSE);
			Logger::log("\t - Transfer: " + has_support(TRANSFER), Logger::VERBOSE);
			Logger::log("\t - Present:  " + has_support(PRESENT), Logger::VERBOSE);
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

	uint32_t index = 0;
	for (auto &vk_queue_family : vk_queue_families) {
		queue_families.push_back(QueueFamily(vk_queue_family, index++, *this, surface));
		QueueFamily& queue_family = queue_families.back();

		if (queue_family.supports_graphics && !selected_family.contains(GRAPHICS)) selected_family.insert(std::pair(GRAPHICS, queue_family));
		if (queue_family.supports_compute && !selected_family.contains(COMPUTE)) selected_family.insert(std::pair(COMPUTE, queue_family));
		if (queue_family.supports_transfer && !selected_family.contains(TRANSFER)) selected_family.insert(std::pair(TRANSFER, queue_family));
		if (queue_family.supports_present && !selected_family.contains(PRESENT)) selected_family.insert(std::pair(PRESENT, queue_family));
	}
}

VkPhysicalDevice &PhysicalDevice::get() {
	return device;
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