#pragma once

#include "vulkan/vulkan.h"
#include <vector>
#include <optional>

class AttachmentDescriptions {
public:
	std::vector<VkAttachmentDescription> attachment_descriptions;
	std::vector<VkAttachmentReference> color_attachment_references;
	std::optional<VkAttachmentReference> depth_attachment_reference;

	void add_attachment(VkFormat format, bool store = true);
};

