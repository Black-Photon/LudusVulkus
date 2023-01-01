#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "PhysicalDevice.h"
#include "Surface.h"

class SwapChainDetails {
public:
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;

    SwapChainDetails(PhysicalDevice &device, Surface &surface);
};

