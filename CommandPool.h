#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"
#include "CommandBuffer.h"

class CommandPool : public std::enable_shared_from_this<CommandPool> {
public:
	std::vector<std::unique_ptr<CommandBuffer>> command_buffers;

	CommandPool(Device &device);
	~CommandPool();

	VkCommandPool get();

	CommandBuffer &create_command_buffer();

private:
	Device &device;
	VkCommandPool command_pool;
};

