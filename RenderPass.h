#pragma once

#include <vulkan/vulkan.h>

#include "SwapChain.h"
#include "SubpassDependency.h"

class RenderPass {
public:
	RenderPass(Device &device, SwapChain& swap_chain, std::vector<SubpassDependency> dependancies = {});
	~RenderPass();

	VkRenderPass get();

private:
	Device &device;
	VkRenderPass render_pass;
};
