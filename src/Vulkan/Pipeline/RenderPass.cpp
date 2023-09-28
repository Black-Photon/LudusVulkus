#include "RenderPass.h"
#include "Logger.h"

RenderPass::RenderPass(Device &device, AttachmentDescriptions attachment_descriptions, std::vector<SubpassDependency> dependencies) :
    device(device)
{
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = attachment_descriptions.color_attachment_references.size();
    subpass.pColorAttachments = attachment_descriptions.color_attachment_references.data();
    if (attachment_descriptions.depth_attachment_reference.has_value()) {
        subpass.pDepthStencilAttachment = &attachment_descriptions.depth_attachment_reference.value();
    }

    std::vector<VkSubpassDependency> vk_dependencies;
    for (SubpassDependency& dependency : dependencies) {
        vk_dependencies.push_back(dependency.get());
    }

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = attachment_descriptions.attachment_descriptions.size();
    render_pass_info.pAttachments = attachment_descriptions.attachment_descriptions.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = vk_dependencies.size();
    render_pass_info.pDependencies = vk_dependencies.data();

    if (vkCreateRenderPass(device.get(), &render_pass_info, nullptr, &render_pass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass");
    }
}

RenderPass::~RenderPass() {
    Logger::log("Freeing Render Pass", Logger::VERBOSE);
    vkDestroyRenderPass(device.get(), render_pass, nullptr);
}

VkRenderPass RenderPass::get() {
    return render_pass;
}