#include "DescriptorPool.h"

#include <exception>

#include "Helper.h"

DescriptorPool::DescriptorPool(Device &device, std::vector<DescriptorSetInfo>& descriptor_set_infos, uint32_t descriptor_count) : device(device), descriptor_set_infos(descriptor_set_infos), descriptor_count(descriptor_count) {
	uint32_t max_descriptor_count = 0;

	std::vector<VkDescriptorPoolSize> pool_sizes{ descriptor_set_infos.size() };
	for (uint32_t i = 0; i < descriptor_set_infos.size(); i++) {
		DescriptorSetInfo descriptor_set_info = descriptor_set_infos[i];
		pool_sizes[i].type = get_access_type(descriptor_set_info.descriptor_type);
		pool_sizes[i].descriptorCount = descriptor_count;

		if (descriptor_count > max_descriptor_count) {
			max_descriptor_count = descriptor_count;
		}
	}

	VkDescriptorPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = pool_sizes.size();
	pool_info.pPoolSizes = pool_sizes.data();
	pool_info.maxSets = max_descriptor_count;

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

void DescriptorPool::update_descriptor_sets(std::vector<DescriptorAccess>& data) {
	if (data.size() != descriptor_set_infos.size()) {
		throw std::runtime_error("The number of buffers sets must match the number of descriptor sets");
	}

	using DescriptorInfo = std::variant<VkDescriptorBufferInfo, VkDescriptorImageInfo>;

	for (size_t i = 0; i < descriptor_count; i++) {
		std::vector<VkWriteDescriptorSet> descriptor_writes{};
		boost::ptr_vector<DescriptorInfo> descriptor_infos{};

		for (uint32_t j = 0; j < descriptor_set_infos.size(); j++) {
			auto& descriptor_set_access = data[j];
			auto& descriptor_set_info = descriptor_set_infos[j];

			VkWriteDescriptorSet descriptor_write{};
			descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_write.dstSet = descriptor_sets[i];
			descriptor_write.dstBinding = j;
			descriptor_write.dstArrayElement = 0;
			descriptor_write.descriptorCount = 1;
			descriptor_write.descriptorType = get_access_type(descriptor_set_info.descriptor_type);

			switch (descriptor_set_info.descriptor_type) {
			case DescriptorType::Sampler:
			{
				if (!std::holds_alternative<boost::ptr_vector<Buffer> *>(descriptor_set_access)) {
					throw std::runtime_error("DescriptorPool::update_descriptor_sets must be given a ptr_vector<Buffer> if DescriptorType::Sampler is used");
				}
				if (std::get<boost::ptr_vector<Buffer> *>(descriptor_set_access)->size() != descriptor_count) {
					throw std::runtime_error("The number of buffers must match the descriptor pool size");
				}

				auto& descriptor_set_buffers = *std::get<boost::ptr_vector<Buffer> *>(descriptor_set_access);

				VkDescriptorBufferInfo buffer_info{};
				descriptor_infos.push_back(new DescriptorInfo{ buffer_info });
				buffer_info.buffer = descriptor_set_buffers[i].get();
				buffer_info.offset = 0;
				buffer_info.range = descriptor_set_info.descriptor_size;
				descriptor_write.pBufferInfo = &buffer_info;
				break;
			}
			case DescriptorType::CombinedImageSampler:
			{
				if (!std::holds_alternative<ImageSampler>(descriptor_set_access)) {
					throw std::runtime_error("DescriptorPool::update_descriptor_sets must be given a ptr_vector<Buffer> if DescriptorType::Sampler is used");
				}

				auto& descriptor_set_sampler = std::get<ImageSampler>(descriptor_set_access);

				VkDescriptorImageInfo image_info{};
				descriptor_infos.push_back(new DescriptorInfo{ image_info });
				image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				image_info.imageView = descriptor_set_sampler.image_view;
				image_info.sampler = descriptor_set_sampler.sampler;
				descriptor_write.pImageInfo = &image_info;
				break;
			}
			default:
				throw std::runtime_error("Unknown descriptor type");
			}

			descriptor_writes.push_back(descriptor_write);
		}

		vkUpdateDescriptorSets(device.get(), descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
	}
}