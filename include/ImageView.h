#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "Device.h"

class ImageView {
public:
	ImageView(Device &device, VkImage &image, VkFormat image_format);
	ImageView(const ImageView&) = delete;
	~ImageView();

	VkImageView get();
private:
	Device &device;
	VkImageView image_view;
};

