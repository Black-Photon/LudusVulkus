#include "Buffer.h"

Buffer::Buffer(Device& device, const size_t buffer_size, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties) : device(device) {
	VkBufferCreateInfo buffer_info{};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = buffer_size;
	buffer_info.usage = buffer_usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device.get(), &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create buffer");
	}

	vkGetBufferMemoryRequirements(device.get(), buffer, &memory_requirements);

	VkMemoryAllocateInfo memory_alloc_info{};
	memory_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_alloc_info.allocationSize = memory_requirements.size;
	memory_alloc_info.memoryTypeIndex = device.physical_device.find_memory_type(memory_requirements.memoryTypeBits, memory_properties);

	if (vkAllocateMemory(device.get(), &memory_alloc_info, nullptr, &device_memory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate memory");
	}

	vkBindBufferMemory(device.get(), buffer, device_memory, 0);
}

Buffer::~Buffer() {
	Logger::log("Freeing Buffer", Logger::VERBOSE);
	vkDestroyBuffer(device.get(), buffer, nullptr);
	vkFreeMemory(device.get(), device_memory, nullptr);
}

VkBuffer& Buffer::get() {
	return buffer;
}

void Buffer::fill_buffer(const void* data, size_t data_size, uint32_t offset) {
	// We'll assume they want to fill in the data iff the host can access the memory
	void* mapped_memory;
	vkMapMemory(device.get(), device_memory, offset, data_size, 0, &mapped_memory);
	memcpy(mapped_memory, data, data_size);
	vkUnmapMemory(device.get(), device_memory);
}