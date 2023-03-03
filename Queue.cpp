#include "Queue.h"

#include "Device.h"
#include "Helper.h"

Queue::Queue(QueueFamily &queue_family) : queue_family(queue_family) {
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = queue_family.index;
	queue_create_info.queueCount = 1;
	queue_create_info.pQueuePriorities = &priority;
}

void Queue::setup_queue(Device &device) {
	VkQueue queue;
	vkGetDeviceQueue(device.get(), queue_family.index, 0, &queue);
	this->queue = queue;
}

VkQueue& Queue::get() {
	return queue.value();
}

void Queue::submit(CommandBuffer &command_buffer, std::vector<std::pair<Semaphore *, PipelineStage>> &wait_semaphores, std::vector<Semaphore *> &signal_semaphores, std::optional<Fence *> fence) {
	assert_setup();
	std::vector<VkSemaphore> vk_wait_semaphores;
	std::vector<VkPipelineStageFlags> vk_pipeline_stages;
	for (auto& pair : wait_semaphores) {
		vk_wait_semaphores.push_back(pair.first->get());
		vk_pipeline_stages.push_back(get_pipeline_stage_flags(pair.second));
	}

	std::vector<VkSemaphore> vk_signal_semaphores;
	for (auto& semaphore : signal_semaphores) {
		vk_signal_semaphores.push_back(semaphore->get());
	}

	VkCommandBuffer vk_command_buffer = command_buffer.get();

	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = wait_semaphores.size();
	submit_info.pWaitSemaphores = !wait_semaphores.empty() ? vk_wait_semaphores.data() : VK_NULL_HANDLE;
	submit_info.pWaitDstStageMask = !wait_semaphores.empty() ? vk_pipeline_stages.data() : VK_NULL_HANDLE;

	submit_info.signalSemaphoreCount = signal_semaphores.size();
	submit_info.pSignalSemaphores = !signal_semaphores.empty() ? vk_signal_semaphores.data() : VK_NULL_HANDLE;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &vk_command_buffer;

	VkFence vk_fence = fence.has_value() ? fence.value()->get() : VK_NULL_HANDLE;
	
	if (vkQueueSubmit(queue.value(), 1, &submit_info, vk_fence) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit command buffer");
	}
}

void Queue::present(SwapChain& swap_chain, uint32_t index, std::vector<Semaphore *> &wait_semaphores) {
	assert_setup();
	if (!queue_family.supports_present) {
		throw std::runtime_error("Attempting to present to queue without present capibilities");
	}

	std::vector<VkSemaphore> vk_wait_semaphores;
	for (auto& semaphore : wait_semaphores) {
		vk_wait_semaphores.push_back(semaphore->get());
	}

	VkPresentInfoKHR present_info{};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	present_info.waitSemaphoreCount = wait_semaphores.size();
	present_info.pWaitSemaphores = !wait_semaphores.empty() ? vk_wait_semaphores.data() : VK_NULL_HANDLE;

	VkSwapchainKHR swap_chains[] = { swap_chain.get() };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swap_chains;
	present_info.pImageIndices = &index;
	present_info.pResults = nullptr;

	VkResult result = vkQueuePresentKHR(queue.value(), &present_info);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		throw SwapChainOutdated();
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present image");
	}

}

void Queue::assert_setup() {
	if (!queue.has_value()) {
		throw std::runtime_error("'setup_queue' must be called before using the queue");
	}
}