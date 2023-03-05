#include "TriangleRenderPass.h"

#include "Logger.h"

TriangleRenderPass::TriangleRenderPass(Device& device, SwapChain& swap_chain, std::vector<SubpassDependency> dependancies) :
    device(device), swap_chain(&swap_chain)
{
    SubpassDependency dependancy;
    dependancy.set_src_subpass(VK_SUBPASS_EXTERNAL);
    dependancy.set_dest_subpass(0);
    dependancy.add_src_stage(ColourAttachmentOutput); // Only when previous has finished writing...
    dependancy.add_dest_stage(ColourAttachmentOutput); // will we write the current subpass
    dependancy.add_dest_access(ColourAttachmentWrite); // Only write color

    render_pass = std::make_unique<RenderPass>(device, swap_chain.image_format, std::vector { dependancy });
}

void TriangleRenderPass::update_swapchain(SwapChain& swap_chain) {
    this->swap_chain = &swap_chain;
}

void TriangleRenderPass::prepare_framebuffers() {
    framebuffers = std::vector<std::unique_ptr<Framebuffer>>();
    if (swap_chain->image_views.size() == 0) {
        throw std::runtime_error("Render pass has no targets!");
    }
    for (auto& image_view : swap_chain->image_views) {
        auto framebuffer = std::make_unique<Framebuffer>(device, *render_pass, *image_view, *swap_chain);
        Logger::log("Adding framebuffer", Logger::VERBOSE);
        framebuffers.push_back(std::move(framebuffer));
    }
}

void TriangleRenderPass::prepare_pipeline() {
    Shader vertex_shader(device, "Vertex.spv");
    Shader fragment_shader(device, "Fragment.spv");
    pipeline = std::make_unique<Pipeline>(vertex_shader, fragment_shader, device, *render_pass);
}

void TriangleRenderPass::record_commands(CommandBuffer& command_buffer, uint32_t current_framebuffer) {
    command_buffer.cmd_begin_render_pass(*render_pass, *framebuffers.at(current_framebuffer));
    command_buffer.cmd_bind_pipeline(*pipeline);
    command_buffer.cmd_set_scissor();
    command_buffer.cmd_set_viewport();
    command_buffer.cmd_draw();
    command_buffer.cmd_end_render_pass();
}