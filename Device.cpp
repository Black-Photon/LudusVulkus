#include "Device.h"

#include "Logger.h"

Device::Device(PhysicalDevice &physical_device,
			   QueueFamily& queue_family,
			   Settings &settings,
			   std::vector<std::string> required_layers) {
	std::map<QueueFamily, std::shared_ptr<Queue>> created_queues;
	std::vector<VkDeviceQueueCreateInfo> created_queue_info;

	for (auto pair : physical_device.selected_family) {
		std::shared_ptr<Queue> queue;
		if (created_queues.contains(pair.second)) {
			queue = created_queues.at(pair.second);
		} else {
			queue = std::make_shared<Queue>(pair.second);
			created_queues.insert(std::pair(pair.second, queue));
			created_queue_info.push_back(queue->queue_create_info);
		}

		queues.insert(std::pair(pair.first, queue));
	}

	VkDeviceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount = static_cast<uint32_t>(created_queue_info.size());
	create_info.pQueueCreateInfos = created_queue_info.data();

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

	for (auto& pair : created_queues) {
		auto queue = pair.second;
		queue->setup_queue(*this);
	}
}

Device::~Device() {
	vkDestroyDevice(device, nullptr);
}

VkDevice& Device::get() {
	return device;
}