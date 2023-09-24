#pragma once

#include <vulkan/vulkan.h>

#include "SwapChain.h"
#include "SubpassDependency.h"
#include "AttachmentDescriptions.h"

class RenderPass {
public:
	RenderPass(Device &device, AttachmentDescriptions attachment_descriptions, std::vector<SubpassDependency> dependencies = {});
	~RenderPass();

	VkRenderPass get();

private:
	Device &device;
	VkRenderPass render_pass;
};

