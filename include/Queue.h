#pragma once

#include <vulkan/vulkan.h>
#include <optional>

#include "QueueFamily.h"
#include "CommandBuffer.h"
#include "Semaphore.h"
#include "Type.h"
#include "SwapChain.h"
#include "Fence.h"

#include <optional>

class Device;

class Queue {
public:
	VkDeviceQueueCreateInfo queue_create_info{};

	Queue(QueueFamily& queue_family);

	void setup_queue(Device &device);
	void submit(CommandBuffer& command_buffer);
	void submit(CommandBuffer &command_buffer, std::vector<std::pair<Semaphore *, VkPipelineStageFlags>> &wait_semaphores, std::vector<Semaphore *> &signal_semaphores, std::optional<Fence *> fence = std::nullopt);
	void present(SwapChain& swap_chain, uint32_t index, std::vector<Semaphore *>& wait_semaphores);
	void wait_idle();

	VkQueue& get();

	QueueFamily queue_family;

private:
	const float priority = 1.0f;
	
	std::optional<VkQueue> queue;

	void assert_setup();
};

