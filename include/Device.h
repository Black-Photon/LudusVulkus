#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <set>

#include "PhysicalDevice.h"
class Settings;
class QueueFamily;
class Queue;
enum QueueType;

class Device {
public:
	Device(const PhysicalDevice &physical_device,
		const QueueFamily& queue_family,
		const Settings& settings,
		const std::set<std::string> &required_extensions,
		const std::set<std::string> &required_layers);
	Device(const Device&) = delete;
	~Device();

	VkDevice& get();

	void wait_idle();

	PhysicalDevice physical_device;
	std::map<QueueType, std::shared_ptr<Queue>> queues;

private:
	VkDevice device;
};

