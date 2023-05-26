#pragma once

#include <vulkan/vulkan.h>
#include <boost/ptr_container/ptr_vector.hpp>

#include "SwapChainDetails.h"
#include "Device.h"
#include "Image.h"
#include "Semaphore.h"
#include "Fence.h"

using ImageIndex = uint32_t;

class SwapChainOutdated : public std::exception {
public:
	char* what() {
		return (char *) "Swapchain is outdated and must be regenerated";
	}
};

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

	bool out_of_date = false;
	VkFormat image_format;
	boost::ptr_vector<Image> images;

private:
	Device &device;
	VkSwapchainKHR swap_chain;
	VkExtent2D extent;

	ImageIndex get_next_image(VkSemaphore semaphore, VkFence fence);
};