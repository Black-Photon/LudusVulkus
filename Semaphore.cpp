#include "Semaphore.h"

#include <vulkan/vulkan.h>

#include "Logger.h"

Semaphore::Semaphore(Device &device) : device(device) {
	VkSemaphoreCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(device.get(), &create_info, nullptr, &semaphore) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create semaphore");
	}
}

Semaphore::~Semaphore() {
	Logger::log("Freeing Semaphore", Logger::VERBOSE);
	vkDestroySemaphore(device.get(), semaphore, nullptr);
}

VkSemaphore Semaphore::get() {
	return semaphore;
}