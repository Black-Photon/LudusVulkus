#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "Device.h"

class Buffer {
public:
	template <class T>
	Buffer(Device &device, const std::vector<T>& data) : device(device) {
		uint32_t buffer_size = sizeof(data[0]) * data.size();

		VkBufferCreateInfo buffer_info{};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = buffer_size;
		buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device.get(), &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create buffer");
		}

		vkGetBufferMemoryRequirements(device.get(), buffer, &memory_requirements);

		VkMemoryAllocateInfo memory_alloc_info{};
		memory_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memory_alloc_info.allocationSize = memory_requirements.size;
		memory_alloc_info.memoryTypeIndex = device.physical_device.find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(device.get(), &memory_alloc_info, nullptr, &device_memory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate memory");
		}

		vkBindBufferMemory(device.get(), buffer, device_memory, 0);

		void* mapped_memory;
		vkMapMemory(device.get(), device_memory, 0, buffer_size, 0, &mapped_memory);
		memcpy(mapped_memory, data.data(), static_cast<size_t>(buffer_size));
		vkUnmapMemory(device.get(), device_memory);
	}
	~Buffer() {
		vkDestroyBuffer(device.get(), buffer, nullptr);
		vkFreeMemory(device.get(), device_memory, nullptr);
	}

	VkBuffer &get() {
		return buffer;
	}

private:
	Device &device;
	VkBuffer buffer;
	VkMemoryRequirements memory_requirements;
	VkDeviceMemory device_memory;

};

