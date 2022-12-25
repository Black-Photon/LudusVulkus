#include "QueueFamily.h"

#include "Logger.h"
#include "PhysicalDevice.h"

QueueFamily::QueueFamily(VkQueueFamilyProperties vk_queue_family, uint32_t index, PhysicalDevice &physical_device, Surface &surface) : index(index) {
	if (vk_queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) supports_graphics = true;
	if (vk_queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT) supports_compute = true;
	if (vk_queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT) supports_transfer = true;
	vkGetPhysicalDeviceSurfaceSupportKHR(physical_device.get(), index, surface.get(), (VkBool32 *) &supports_present);
}
