#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"
#include "RenderPass.h"

class Framebuffer {
public:
	Framebuffer(Device &device, RenderPass& render_pass, Image& image, SwapChain& swap_chain);
	~Framebuffer();

	VkFramebuffer get();

	VkExtent2D extent;

private:
	Device &device;
	VkFramebuffer framebuffer;
};

