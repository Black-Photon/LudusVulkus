#pragma once

#include <vulkan/vulkan.h>
#include <optional>

#include "QueueFamily.h"

class Device;

class Queue {
public:
	VkDeviceQueueCreateInfo queue_create_info{};

	Queue(QueueFamily& queue_family);

	void setup_queue(Device &device);

	VkQueue& get();

private:
	const float priority = 1.0f;
	
	QueueFamily queue_family;
	std::optional<VkQueue> queue;
};

