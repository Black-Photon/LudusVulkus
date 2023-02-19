#include "Fence.h"

#include <vulkan/vulkan.h>

#include "Logger.h"

Fence::Fence(Device& device) : device(device) {
	VkFenceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateFence(device.get(), &create_info, nullptr, &fence) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create fence");
	}
}

Fence::~Fence() {
	Logger::log("Freeing Fence", Logger::VERBOSE);
	vkDestroyFence(device.get(), fence, nullptr);
}

VkFence Fence::get() {
	return fence;
}

void Fence::wait(uint64_t timeout) {
	vkWaitForFences(device.get(), 1, &fence, VK_TRUE, timeout);
}

void Fence::reset() {
	vkResetFences(device.get(), 1, &fence);
}

void Fence::wait_all(std::vector<Fence>& fences, uint64_t timeout) {
	wait_internal(fences, timeout, VK_TRUE);
}

void Fence::wait_any(std::vector<Fence>& fences, uint64_t timeout) {
	wait_internal(fences, timeout, VK_FALSE);
}

void Fence::wait_internal(std::vector<Fence>& fences, uint64_t timeout, VkBool32 wait_all) {
	std::vector<VkFence> vk_fences;
	for (auto& fence : fences) {
		vk_fences.push_back(fence.get());
	}
	vkWaitForFences(fences.at(0).device.get(), vk_fences.size(), vk_fences.data(), VK_TRUE, timeout);
}