#pragma once

#include <vulkan/vulkan.h>

#include "SwapChainDetails.h"
#include "Device.h"
#include "ImageView.h"

class SwapChain {
public:
	static VkSurfaceFormatKHR default_surface_format(const std::vector<VkSurfaceFormatKHR>& formats);
	static VkPresentModeKHR default_presentation_mode(const std::vector<VkPresentModeKHR>& present_modes, Settings& settings);
	static VkExtent2D default_extent(const VkSurfaceCapabilitiesKHR& capabilities, Window& window);

	SwapChain(std::shared_ptr<Device> device, Window& window, Surface& surface, Settings& settings);
	~SwapChain();

	VkSwapchainKHR get();

	std::vector<VkImage> images;
	std::vector<std::unique_ptr<ImageView>> image_views;

private:
	std::shared_ptr<Device> device;
	VkSwapchainKHR swap_chain;
	VkFormat image_format;
	VkExtent2D extent;
};

