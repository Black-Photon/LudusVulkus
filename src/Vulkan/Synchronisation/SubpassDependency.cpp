#include "SubpassDependency.h"

#include "Helper.h"

SubpassDependency::SubpassDependency() {

}

VkSubpassDependency SubpassDependency::get() {
	return dependency;
}

void SubpassDependency::set_src_subpass(uint32_t source) {
	dependency.srcSubpass = source;
}

void SubpassDependency::set_dest_subpass(uint32_t destination) {
	dependency.dstSubpass = destination;
}

void SubpassDependency::add_src_stage(PipelineStage flag) {
	// TODO technically this is a toggle
	dependency.srcStageMask |= get_pipeline_stage_flags(flag);
}

void SubpassDependency::add_dest_stage(PipelineStage flag) {
	dependency.dstStageMask |= get_pipeline_stage_flags(flag);
}

void SubpassDependency::add_src_access(PipelineAccess flag) {
	dependency.srcAccessMask |= get_pipeline_access_flags(flag);
}

void SubpassDependency::add_dest_access(PipelineAccess flag) {
	dependency.dstAccessMask |= get_pipeline_access_flags(flag);
}