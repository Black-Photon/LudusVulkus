#include "AttributeDescriptor.h"

AttributeDescriptor::AttributeDescriptor(std::vector<AttributeEntry> attribute_entries) {
	uint32_t current_pos = 0;
	for (uint32_t i = 0; i < attribute_entries.size(); i++) {
		AttributeEntry& attribute_entry = attribute_entries.at(i);
		VkVertexInputAttributeDescription attribute_descriptor{};
		attribute_descriptor.binding = 0;
		attribute_descriptor.location = i;
		attribute_descriptor.format = attribute_entry.format;
		attribute_descriptor.offset = current_pos;
		attribute_descriptors.push_back(attribute_descriptor);

		current_pos += attribute_entry.attribute_size;
	}

	binding_descriptor = {};
	binding_descriptor.binding = 0;
	binding_descriptor.stride = current_pos;
	binding_descriptor.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}