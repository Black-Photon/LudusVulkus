#include "Buffer.h"

Buffer::Buffer(Device& device, const VkDeviceSize buffer_size, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties, LocalMemoryAllocation local_memory_allocation) : device(device), buffer_size(buffer_size){
	if ((memory_properties & MemoryProperties::HostVisible) == 0 && local_memory_allocation == LocalMemory::Persistent) {
		throw std::runtime_error("Local memory allocation must be set to LocalMemory::Dynamic if memory is not visible to the host");
	}
	
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

	if (local_memory_allocation == LocalMemory::Persistent) {
		void* mapped_memory;
		vkMapMemory(device.get(), device_memory, 0, buffer_size, 0, &mapped_memory);
		this->mapped_memory = mapped_memory;
	}
}

Buffer::~Buffer() {
	Logger::log("Freeing Buffer", Logger::VERBOSE);
	if (mapped_memory.has_value()) {
		vkUnmapMemory(device.get(), device_memory);
	}
	vkDestroyBuffer(device.get(), buffer, nullptr);
	vkFreeMemory(device.get(), device_memory, nullptr);
}

const VkBuffer& Buffer::get() const {
	return buffer;
}

void Buffer::fill_buffer(const void* data, VkDeviceSize data_size, uint32_t offset) {
	// We'll assume they want to fill in the data iff the host can access the memory
	if (offset + data_size > buffer_size) {
		throw std::runtime_error("The fill range specified exceeds the bounds of the buffer");
	}

	bool is_persistent = this->mapped_memory.has_value();
	void* mapped_memory;
	if (!is_persistent) {
		vkMapMemory(device.get(), device_memory, offset, data_size, 0, &mapped_memory);
	} else {
		mapped_memory = this->mapped_memory.value();
	}
	memcpy(mapped_memory, data, data_size);
	if (!is_persistent) {
		vkUnmapMemory(device.get(), device_memory);
	}
}