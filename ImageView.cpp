#include "ImageView.h"

ImageView::ImageView(std::shared_ptr<Device> device, VkImage& image, VkFormat image_format) :
	device(device)
{
	VkImageViewCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	create_info.image = image;
	create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	create_info.format = image_format;
	create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	create_info.subresourceRange.baseMipLevel = 0;
	create_info.subresourceRange.levelCount = 1;
	create_info.subresourceRange.baseArrayLayer = 0;
	create_info.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device->get(), &create_info, nullptr, &image_view) != VK_SUCCESS) {
		throw std::runtime_error("Could not create image views");
	}
}

ImageView::~ImageView() {
	vkDestroyImageView(device->get(), image_view, nullptr);
}

VkImageView ImageView::get() {
	return image_view;
}