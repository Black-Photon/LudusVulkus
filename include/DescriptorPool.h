#pragma once

#include <vulkan/vulkan.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <variant>

#include "Device.h"
#include "Buffer.h"
#include "DescriptorSetInfo.h"

class DescriptorPool {
public:
	struct ImageSampler {
		VkImageView image_view;
		VkSampler sampler;
	};

	using DescriptorAccess = std::variant<boost::ptr_vector<Buffer> *, ImageSampler>;

	DescriptorPool(Device& device, std::vector<DescriptorSetInfo>& descriptor_set_infos, uint32_t descriptor_count);
	~DescriptorPool();

	VkDescriptorSet get_descriptor_set(uint32_t index);

	void allocate_descriptor_set(VkDescriptorSetLayout descriptor_set_layout);
	void update_descriptor_sets(std::vector<DescriptorAccess>& data);


private:
	Device& device;

	VkDescriptorPool descriptor_pool;
	std::vector<DescriptorSetInfo> descriptor_set_infos;
	std::vector<VkDescriptorSet> descriptor_sets;
	uint32_t descriptor_count;
};

