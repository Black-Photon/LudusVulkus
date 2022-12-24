#pragma once

#include <vulkan/vulkan.h>
#include <optional>

class QueueFamily {
public:
	QueueFamily(VkQueueFamilyProperties vk_queue_family, uint32_t index);

	uint32_t index;
	bool supports_graphics = false;
	bool supports_compute = false;
	bool supports_transfer = false;
};

