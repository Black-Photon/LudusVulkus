#pragma once

#include <vulkan/vulkan.h>
#include <optional>

#include "Surface.h"

class PhysicalDevice;

class QueueFamily {
public:
	QueueFamily(VkQueueFamilyProperties vk_queue_family, uint32_t index, PhysicalDevice& physical_device, Surface& surface);

	bool operator==(const QueueFamily& other) const {
		return index == other.index;
	}
	bool operator<(const QueueFamily & other) const {
		return index < other.index;
	}

	uint32_t index;
	bool supports_graphics = false;
	bool supports_compute = false;
	bool supports_transfer = false;
	bool supports_present = false;
};

