#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "Device.h"
#include "Type.h"
#include "Logger.h"

class Buffer {
public:
	template <class T>
	static std::unique_ptr<Buffer> create_buffer(Device &device, const std::vector<T>& data, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties) {
		if ((memory_properties & MemoryProperties::HostVisible) == 0) {
			throw std::runtime_error("Can't set memory as the host doesn't have visibility - use MemoryProperties::HostVisible or create_empty_buffer");
		}

		size_t buffer_size = sizeof(data[0]) * data.size();
		// Need new here as we've got a private constructor
		Buffer *buffer = new Buffer(device, buffer_size, buffer_usage, memory_properties);
		buffer->fill_buffer(static_cast<const void*>(data.data()), buffer_size);

		return std::unique_ptr<Buffer>(buffer);
	}

	static std::unique_ptr<Buffer> create_empty_buffer(Device& device, const size_t buffer_size, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties) {
		Buffer *buffer = new Buffer(device, buffer_size, buffer_usage, memory_properties);
		return std::unique_ptr<Buffer>(buffer);
	}

	~Buffer();

	VkBuffer& get();

	void fill_buffer(const void* data, size_t data_size, uint32_t offset = 0);

private:
	Buffer(Device& device, const size_t buffer_size, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties);

	Device &device;
	VkBuffer buffer;
	VkMemoryRequirements memory_requirements;
	VkDeviceMemory device_memory;

};

