#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct AttributeEntry {
	VkFormat format;
	uint32_t attribute_size;
};

class AttributeDescriptor {
public:
	AttributeDescriptor(std::vector<AttributeEntry> attribute_entries);

	VkVertexInputBindingDescription binding_descriptor;
	std::vector<VkVertexInputAttributeDescription> attribute_descriptors;
};

