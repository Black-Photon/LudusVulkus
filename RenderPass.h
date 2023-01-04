#pragma once

#include <vulkan/vulkan.h>

#include "SwapChain.h"

class RenderPass {
public:
	RenderPass(std::shared_ptr<Device> device, SwapChain& swap_chain);
	~RenderPass();

	VkRenderPass get();

private:
	std::shared_ptr<Device> device;
	VkRenderPass render_pass;
};

