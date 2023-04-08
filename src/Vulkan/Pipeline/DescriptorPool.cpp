#include "DescriptorPool.h"

#include <exception>

DescriptorPool::DescriptorPool(Device &device, uint32_t descriptor_count) : device(device), descriptor_count(descriptor_count) {
	VkDescriptorPoolSize pool_size{};
	pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_size.descriptorCount = descriptor_count;

	VkDescriptorPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = 1;
	pool_info.pPoolSizes = &pool_size;
	pool_info.maxSets = descriptor_count;

	if (vkCreateDescriptorPool(device.get(), &pool_info, nullptr, &descriptor_pool) != VK_SUCCESS) {
		throw std::runtime_error("Unable to create descriptor pool");
	}
}

DescriptorPool::~DescriptorPool() {
	vkDestroyDescriptorPool(device.get(), descriptor_pool, nullptr);
}

VkDescriptorSet DescriptorPool::get_descriptor_set(uint32_t index) {
	if (index >= descriptor_sets.size()) {
		throw std::runtime_error("Requested descriptor set beyond descriptor pool range");
	}

	return descriptor_sets[index];
}

void DescriptorPool::allocate_descriptor_set(VkDescriptorSetLayout descriptor_set_layout) {
	std::vector<VkDescriptorSetLayout> layouts(descriptor_count, descriptor_set_layout);
	VkDescriptorSetAllocateInfo allocation_info{};
	allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocation_info.descriptorPool = descriptor_pool;
	allocation_info.descriptorSetCount = descriptor_count;
	allocation_info.pSetLayouts = layouts.data();

	descriptor_sets.resize(descriptor_count);
	if (vkAllocateDescriptorSets(device.get(), &allocation_info, descriptor_sets.data()) != VK_SUCCESS) {
		throw std::runtime_error("Unable to allocate descriptor sets");
	}
}

void DescriptorPool::update_descriptor_sets(std::vector<std::unique_ptr<Buffer>>& buffers, uint32_t descriptor_set_size) {
	if (buffers.size() != descriptor_count) {
		throw std::runtime_error("The number of buffers must match the descriptor pool size");
	}

	for (size_t i = 0; i < descriptor_count; i++) {
		VkDescriptorBufferInfo buffer_info{};
		buffer_info.buffer = buffers[i]->get();
		buffer_info.offset = 0;
		buffer_info.range = descriptor_set_size;

		VkWriteDescriptorSet descriptor_write{};
		descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_write.dstSet = descriptor_sets[i];
		descriptor_write.dstBinding = 0;
		descriptor_write.dstArrayElement = 0;
		descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptor_write.descriptorCount = 1;
		descriptor_write.pBufferInfo = &buffer_info;
		descriptor_write.pImageInfo = nullptr;
		descriptor_write.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(device.get(), 1, &descriptor_write, 0, nullptr);
	}
}