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

void SubpassDependency::add_src_stage(VkPipelineStageFlags flag) {
	// TODO technically this is a toggle
	dependency.srcStageMask |= flag;
}

void SubpassDependency::add_dest_stage(VkPipelineStageFlags flag) {
	dependency.dstStageMask |= flag;
}

void SubpassDependency::add_src_access(VkAccessFlags flag) {
	dependency.srcAccessMask |= flag;
}

void SubpassDependency::add_dest_access(VkAccessFlags flag) {
	dependency.dstAccessMask |= flag;
}