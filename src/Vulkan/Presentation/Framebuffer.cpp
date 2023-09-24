#include "Framebuffer.h"

#include "Logger.h"

Framebuffer::Framebuffer(Device &device, RenderPass &render_pass, std::vector<Image*> attachments, SwapChain &swap_chain) :
	device(device), extent(swap_chain.get_extent())
{
    std::vector<VkImageView> views{};
    for (auto &attachment : attachments) {
        views.push_back(attachment->get_view());
    }

    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass.get();
    framebuffer_info.attachmentCount = views.size();
    framebuffer_info.pAttachments = views.data();
    framebuffer_info.width = swap_chain.get_extent().width;
    framebuffer_info.height = swap_chain.get_extent().height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(device.get(), &framebuffer_info, nullptr, &framebuffer) != VK_SUCCESS) {
        throw std::runtime_error("Unable to create framebuffer");
    }
}

Framebuffer::~Framebuffer() {
    Logger::log("Freeing Framebuffer", Logger::VERBOSE);
	vkDestroyFramebuffer(device.get(), framebuffer, nullptr);
}

VkFramebuffer Framebuffer::get() {
	return framebuffer;
}