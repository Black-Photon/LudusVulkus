#pragma once

#include <vulkan/vulkan.hpp>

struct DescriptorSetInfo {
	VkShaderStageFlags shader_stage;
	VkDescriptorType descriptor_type;
	uint32_t descriptor_size;
};