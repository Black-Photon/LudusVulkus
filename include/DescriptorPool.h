#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"
#include "Buffer.h"

class DescriptorPool {
public:
	DescriptorPool(Device& device, uint32_t descriptor_count);
	~DescriptorPool();

	VkDescriptorSet get_descriptor_set(uint32_t index);

	void allocate_descriptor_set(VkDescriptorSetLayout descriptor_set_layout);
	void update_descriptor_sets(std::vector<std::unique_ptr<Buffer>>& buffers, uint32_t descriptor_set_size);

private:
	Device& device;

	VkDescriptorPool descriptor_pool;
	std::vector<VkDescriptorSet> descriptor_sets;
	uint32_t descriptor_count;
};

