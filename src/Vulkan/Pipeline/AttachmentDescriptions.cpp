#include "AttachmentDescriptions.h"

#include <stdexcept>

#include "Helper.h"

void AttachmentDescriptions::add_attachment(VkFormat format, bool store) {
	bool is_depth_stencil = has_depth(format) || has_stencil(format);

	VkAttachmentDescription attachment_description{};
	attachment_description.format = format;
	attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description.storeOp = store ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	if (is_depth_stencil) {
		attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}
	else {
		attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}

	attachment_descriptions.push_back(attachment_description);

	VkAttachmentReference attachment_reference{};
	attachment_reference.attachment = color_attachment_references.size() + (depth_attachment_reference.has_value() ? 1 : 0);
	if (is_depth_stencil) {
		attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}
	else {
		attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	if (is_depth_stencil) {
		if (depth_attachment_reference != std::nullopt) throw std::invalid_argument("Only one depth attachment is allowed");
		depth_attachment_reference = attachment_reference;
	}
	else {
		color_attachment_references.push_back(attachment_reference);
	}
}