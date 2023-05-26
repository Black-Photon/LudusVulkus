#include "Image.h"

Image::Image(const Device& device, VkImage vk_image, const VkFormat format) : device(device), image(vk_image), manage_image_memory(false) {
	create_image_view(format);
}

Image::Image(const Device &device, const VkFormat format, uint32_t width, uint32_t height) : device(device), manage_image_memory(true) {
	VkImageCreateInfo image_info {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = width;
	image_info.extent.height = height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.format = format;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.flags = 0; // Optional

	if (vkCreateImage(device.get(), &image_info, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("Unable to create image");
	}

	VkMemoryRequirements requirements;
	vkGetImageMemoryRequirements(device.get(), image, &requirements);

	VkMemoryAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocate_info.allocationSize = requirements.size;
	allocate_info.memoryTypeIndex = device.physical_device.find_memory_type(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(device.get(), &allocate_info, nullptr, &memory) != VK_SUCCESS) {
		throw std::runtime_error("Unable to allocate image memory");
	}

	vkBindImageMemory(device.get(), image, memory, 0);

	create_image_view(format);
}

Image::~Image() {
	Logger::log("Freeing Image", Logger::VERBOSE);
	vkDestroyImageView(device.get(), image_view, nullptr);
	if (manage_image_memory) {
		vkDestroyImage(device.get(), image, nullptr);
		vkFreeMemory(device.get(), memory, nullptr);
	}
}

const VkImage Image::get() const {
	return image;
}

const VkImageView Image::get_view() const {
	return image_view;
}

void Image::create_image_view(const VkFormat format) {
	VkImageViewCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	create_info.image = image;
	create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	create_info.format = format;
	create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	create_info.subresourceRange.baseMipLevel = 0;
	create_info.subresourceRange.levelCount = 1;
	create_info.subresourceRange.baseArrayLayer = 0;
	create_info.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device.get(), &create_info, nullptr, &image_view) != VK_SUCCESS) {
		throw std::runtime_error("Could not create view for image");
	}
}