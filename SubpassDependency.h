#pragma once

#include <vulkan/vulkan.h>

#include "Type.h"

class SubpassDependency {
public:
	SubpassDependency();

	VkSubpassDependency get();

	void set_src_subpass(uint32_t source);
	void set_dest_subpass(uint32_t destination);

	void add_src_stage(PipelineStage flag);
	void add_dest_stage(PipelineStage flag);

	void add_src_access(PipelineAccess flag);
	void add_dest_access(PipelineAccess flag);

private:
	VkSubpassDependency dependency{};
};

