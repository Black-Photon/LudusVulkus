#pragma once

#include <vulkan/vulkan.h>
#include <exception>

#include "Device.h"
#include "Logger.h"

class Image {
public:
	Image(const Device& device, VkImage vk_image, const VkFormat format);
	Image(const Device& device, const VkFormat format, uint32_t width, uint32_t height);
	Image(const Image&) = delete;
	~Image();

	const VkImage get() const;
	const VkImageView get_view() const;
private:
	const Device& device;
	VkImage image;
	VkDeviceMemory memory;
	VkImageView image_view;
	bool manage_image_memory;

	void create_image_view(const VkFormat format);
};