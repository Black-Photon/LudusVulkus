#include "Device.h"

Device::Device(PhysicalDevice &physical_device,
			   QueueFamily& queue_family,
			   Settings &settings,
			   std::vector<std::string> required_layers) : queue(queue_family) {   
	VkDeviceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = &queue.queue_create_info;
	create_info.queueCreateInfoCount = 1;

	create_info.pEnabledFeatures = &physical_device.device_features;

	create_info.enabledExtensionCount = 0;

	// Not neccesary in Vulkan 1.3+, but added for compatibility
	std::vector<const char*> c_layers;
	if (settings.use_validation_layers) {
		// Copy validation layer list into new structure
		std::transform(required_layers.begin(), required_layers.end(), std::back_inserter(c_layers), [](auto &string) { return string.c_str(); });

		create_info.enabledLayerCount = c_layers.size();
		create_info.ppEnabledLayerNames = c_layers.data();
	} else {
		create_info.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physical_device.get(), &create_info, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("Could not create device for chosen queue family and physical device");
	}

	queue.setup_queue(*this);
}

Device::~Device() {
	vkDestroyDevice(device, nullptr);
}

VkDevice& Device::get() {
	return device;
}