#include "SwapChain.h"

#include <algorithm>

#include "Logger.h"

VkSurfaceFormatKHR SwapChain::default_surface_format(const std::vector<VkSurfaceFormatKHR>& formats) {
    std::map<VkFormat, uint32_t> format_priority;
    format_priority.insert(std::pair(VK_FORMAT_B8G8R8A8_SRGB, 0));
    format_priority.insert(std::pair(VK_FORMAT_B8G8R8A8_UNORM, 1));
    format_priority.insert(std::pair(VK_FORMAT_B8G8R8A8_SNORM, 2));
    format_priority.insert(std::pair(VK_FORMAT_B8G8R8A8_UINT, 3));
    format_priority.insert(std::pair(VK_FORMAT_B8G8R8A8_SINT, 4));

    std::multimap<uint32_t, VkSurfaceFormatKHR> surface_formats;
    for (const auto& format : formats) {
        if (format.colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            Logger::log("Unknown VkColorSpaceKHR " + std::to_string(format.colorSpace) +
                        " when choosing default surface format", Logger::WARN);
            continue;
        }

        int priority = format_priority.contains(format.format) ? format_priority.at(format.format) : INT_MAX;
        surface_formats.insert(std::pair(priority, format));
    }

    return (*surface_formats.begin()).second;
}

VkPresentModeKHR SwapChain::default_presentation_mode(const std::vector<VkPresentModeKHR>& present_modes, Settings& settings) {
    // Use mailbox if not on mobile as slightly better performance, but at the cost of battery life
    if (std::count(present_modes.begin(), present_modes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != 0 &&
        settings.is_mobile) {
        return VK_PRESENT_MODE_MAILBOX_KHR;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::default_extent(const VkSurfaceCapabilitiesKHR& capabilities, Window &window) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window.get(), &width, &height);

        VkExtent2D extent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return extent;
    }
}

SwapChain::SwapChain(Device &device, Window& window, Surface& surface, Settings& settings) :
    device(device) 
{
    PhysicalDevice& physical_device = device.physical_device;
    SwapChainDetails swap_chain_support(physical_device, surface);
    
    // Set default settings (maybe allow passing as parameters in the future?)
    VkSurfaceFormatKHR surface_format = default_surface_format(swap_chain_support.formats);
    VkPresentModeKHR present_mode = default_presentation_mode(swap_chain_support.present_modes, settings);
    extent = default_extent(swap_chain_support.capabilities, window);
    image_format = surface_format.format;

    uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;

    if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount) {
        image_count = swap_chain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface.get();
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamily &graphics_family = physical_device.selected_family.at(GRAPHICS);
    QueueFamily &present_family = physical_device.selected_family.at(PRESENT);
    uint32_t queue_family_indices[] = { graphics_family.index, present_family.index };

    if (graphics_family != present_family) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform = swap_chain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device.get(), &create_info, nullptr, &swap_chain) != VK_SUCCESS) {
        throw std::runtime_error("Unable to create swapchain");
    }

    vkGetSwapchainImagesKHR(device.get(), swap_chain, &image_count, nullptr);
    images.resize(image_count);
    vkGetSwapchainImagesKHR(device.get(), swap_chain, &image_count, images.data());

    for (auto &image : images) {
        Logger::log("Adding Image View");
        auto image_view = std::make_unique<ImageView>(device, image, image_format);
        image_views.push_back(std::move(image_view));
    }
}

SwapChain::~SwapChain() {
    Logger::log("Freeing Swapchain", Logger::VERBOSE);
    vkDestroySwapchainKHR(device.get(), swap_chain, nullptr);
}

VkSwapchainKHR SwapChain::get() {
    return swap_chain;
}

VkExtent2D SwapChain::get_extent() {
    return extent;
}

ImageIndex SwapChain::get_next_image(Semaphore &semaphore) {
    return get_next_image(semaphore.get(), VK_NULL_HANDLE);
}

ImageIndex SwapChain::get_next_image(Fence& fence) {
    return get_next_image(VK_NULL_HANDLE, fence.get());
}

ImageIndex SwapChain::get_next_image(VkSemaphore semaphore, VkFence fence) {
    ImageIndex index;
    VkResult result = vkAcquireNextImageKHR(device.get(), swap_chain, UINT64_MAX, semaphore, fence, &index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        throw SwapChainOutdated();
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to get the next swapchain image");
    }
    return index;
}