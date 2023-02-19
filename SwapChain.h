#pragma once

#include <vulkan/vulkan.h>

#include "SwapChainDetails.h"
#include "Device.h"
#include "ImageView.h"
#include "Semaphore.h"
#include "Fence.h"

using ImageIndex = uint32_t;

class SwapChain {
public:
	static VkSurfaceFormatKHR default_surface_format(const std::vector<VkSurfaceFormatKHR>& formats);
	static VkPresentModeKHR default_presentation_mode(const std::vector<VkPresentModeKHR>& present_modes, Settings& settings);
	static VkExtent2D default_extent(const VkSurfaceCapabilitiesKHR& capabilities, Window& window);

	SwapChain(Device &device, Window& window, Surface& surface, Settings& settings);
	~SwapChain();

	VkSwapchainKHR get();
	VkExtent2D get_extent();

	ImageIndex get_next_image(Semaphore& semaphore);
	ImageIndex get_next_image(Fence& fence);

	VkFormat image_format;
	std::vector<VkImage> images;
	std::vector<std::unique_ptr<ImageView>> image_views;

private:
	Device &device;
	VkSwapchainKHR swap_chain;
	VkExtent2D extent;
};

