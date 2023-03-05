#include "CommandPool.h"

#include "Queue.h"
#include "Logger.h"
#include <stdexcept>

CommandPool::CommandPool(Device &device) :
	device(device)
{
	VkCommandPoolCreateInfo command_pool_info{};
	command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	command_pool_info.queueFamilyIndex = device.queues.at(GRAPHICS)->queue_family.index;

	if (vkCreateCommandPool(device.get(), &command_pool_info, nullptr, &command_pool) != VK_SUCCESS) {
		throw std::runtime_error("Unable to create command pool");
	}
}

CommandPool::~CommandPool() {
	Logger::log("Freeing Command Pool", Logger::VERBOSE);
	command_buffers.clear();
	vkDestroyCommandPool(device.get(), command_pool, nullptr);
}

VkCommandPool CommandPool::get() {
	return command_pool;
}

CommandBuffer &CommandPool::create_command_buffer() {
	auto command_buffer = std::make_unique<CommandBuffer>(device, *this);
	command_buffers.push_back(std::move(command_buffer));
	return *command_buffers.back();
}