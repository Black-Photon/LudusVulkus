#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "Device.h"

class ImageView {
public:
	ImageView(std::shared_ptr<Device> device, VkImage &image, VkFormat image_format);
	ImageView(const ImageView&) = delete;
	~ImageView();

	VkImageView get();
private:
	std::shared_ptr<Device> device;
	VkImageView image_view;
};

