#include "SwapChainDetails.h"

SwapChainDetails::SwapChainDetails(PhysicalDevice& device, Surface& surface) {
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.get(), surface.get(), &capabilities);

	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device.get(), surface.get(), &format_count, nullptr);

	if (format_count != 0) {
		formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device.get(), surface.get(), &format_count, formats.data());
	}

	uint32_t present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device.get(), surface.get(), &present_mode_count, nullptr);

	if (present_mode_count != 0) {
		present_modes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device.get(), surface.get(), &present_mode_count, present_modes.data());
	}
}