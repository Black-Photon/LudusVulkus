#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <set>

#include "Queue.h"
#include "PhysicalDevice.h"
#include "Settings.h"

class Device {
public:
	Device(PhysicalDevice& physical_device,
		QueueFamily& queue_family,
		Settings& settings,
		std::vector<std::string> required_layers);
	Device(const Device&) = delete;
	~Device();

	VkDevice& get();

private:
	std::map<QueueType, std::shared_ptr<Queue>> queues;
	VkDevice device;
};

