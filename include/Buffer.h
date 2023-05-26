#pragma once

#include <stb_image.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "Device.h"
#include "Type.h"
#include "Logger.h"

using namespace std::literals::string_view_literals;

class Buffer {
public:
	template <class T>
	static std::unique_ptr<Buffer> create_buffer(Device &device, const std::vector<T>& data, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties, LocalMemoryAllocation local_memory_allocation = LocalMemory::Dynamic) {
		if ((memory_properties & MemoryProperties::HostVisible) == 0) {
			throw std::runtime_error("Can't set memory as the host doesn't have visibility - use MemoryProperties::HostVisible or create_empty_buffer");
		}

		VkDeviceSize buffer_size = sizeof(data[0]) * data.size();
		// Need new here as we've got a private constructor
		Buffer *buffer = new Buffer(device, buffer_size, buffer_usage, memory_properties, local_memory_allocation);
		buffer->fill_buffer(static_cast<const void*>(data.data()), buffer_size);

		return std::unique_ptr<Buffer>(buffer);
	}

	static std::unique_ptr<Buffer> create_empty_buffer(Device& device, const VkDeviceSize buffer_size, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties, LocalMemoryAllocation local_memory_allocation = LocalMemory::Dynamic) {
		Buffer *buffer = new Buffer(device, buffer_size, buffer_usage, memory_properties, local_memory_allocation);
		return std::unique_ptr<Buffer>(buffer);
	}

	static std::tuple<std::unique_ptr<Buffer>, uint32_t, uint32_t> create_buffer_from_image(Device& device, const std::string &image_path, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties, LocalMemoryAllocation local_memory_allocation = LocalMemory::Dynamic) {
		if ((memory_properties & MemoryProperties::HostVisible) == 0) {
			throw std::runtime_error("Can't set memory as the host doesn't have visibility - use MemoryProperties::HostVisible or create_empty_buffer");
		}

		int tex_width, tex_height, tex_channels;
		stbi_uc* image_data = stbi_load(image_path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
		VkDeviceSize image_size = tex_width * tex_height * 4;

		if (!image_data) {
			throw std::runtime_error("Failed to load texture from " + image_path);
		}
		
		Buffer* buffer = new Buffer(device, image_size, buffer_usage, memory_properties, local_memory_allocation);
		buffer->fill_buffer(static_cast<const void*>(image_data), image_size);
		stbi_image_free(image_data);

		return std::make_tuple(std::unique_ptr<Buffer>(buffer), tex_width, tex_height);
	}

	~Buffer();

	const VkBuffer& get() const;

	void fill_buffer(const void* data, VkDeviceSize data_size, uint32_t offset = 0);

private:
	Buffer(const Buffer&) = delete;
	Buffer& operator=(Buffer const&) = delete;
	Buffer(Device& device, const VkDeviceSize buffer_size, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties, LocalMemoryAllocation local_memory_allocation);

	Device &device;
	VkBuffer buffer;
	uint32_t buffer_size;
	VkMemoryRequirements memory_requirements;
	VkDeviceMemory device_memory;
	std::optional<void*> mapped_memory;
};

