#include "GeometryRenderPass.h"

#include "Logger.h"

GeometryRenderPass::GeometryRenderPass(Device& device, SwapChain& swap_chain, std::vector<SubpassDependency> dependancies) :
    device(device), swap_chain(&swap_chain)
{
    SubpassDependency dependancy;
    dependancy.set_src_subpass(VK_SUBPASS_EXTERNAL);
    dependancy.set_dest_subpass(0);
    dependancy.add_src_stage(PipelineStage::ColourAttachmentOutput); // Only when previous has finished writing...
    dependancy.add_dest_stage(PipelineStage::ColourAttachmentOutput); // will we write the current subpass
    dependancy.add_dest_access(PipelineAccess::ColourAttachmentWrite); // Only write color

    render_pass = std::make_unique<RenderPass>(device, swap_chain.image_format, std::vector{ dependancy });
}

void GeometryRenderPass::update_swapchain(SwapChain& swap_chain) {
    this->swap_chain = &swap_chain;
}

void GeometryRenderPass::prepare_framebuffers() {
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

void GeometryRenderPass::prepare_pipeline(CommandPool& setup_command_pool, Queue& transfer_queue) {
    Shader vertex_shader(device, "Vertices_vert.spv");
    Shader fragment_shader(device, "Vertices_frag.spv");

    size_t vertex_data_size = sizeof(vertices[0]) * vertices.size();
    auto vertex_staging_buffer = Buffer::create_buffer(device, vertices, BufferUsage::TransferSource, MemoryProperties::HostVisible | MemoryProperties::HostCoherent);
    vertex_buffer = Buffer::create_empty_buffer(device, vertex_data_size, BufferUsage::TransferDestination | BufferUsage::Vertex, MemoryProperties::DeviceLocal);
    CommandBuffer command_buffer(device, setup_command_pool);

    command_buffer.start_recording(true);
    command_buffer.cmd_copy_buffer(*vertex_staging_buffer, *vertex_buffer, vertex_data_size);
    command_buffer.stop_recording();

    transfer_queue.submit(command_buffer);
    transfer_queue.wait_idle();

    size_t index_data_size = sizeof(indices[0]) * indices.size();
    auto index_staging_buffer = Buffer::create_buffer(device, indices, BufferUsage::TransferSource, MemoryProperties::HostVisible | MemoryProperties::HostCoherent);
    index_buffer = Buffer::create_empty_buffer(device, index_data_size, BufferUsage::TransferDestination | BufferUsage::Index, MemoryProperties::DeviceLocal);

    command_buffer.reset();
    command_buffer.start_recording(true);
    command_buffer.cmd_copy_buffer(*index_staging_buffer, *index_buffer, index_data_size);
    command_buffer.stop_recording();

    transfer_queue.submit(command_buffer);
    transfer_queue.wait_idle();

    std::vector<AttributeEntry> attribute_entries;
    attribute_entries.push_back({ VK_FORMAT_R32G32_SFLOAT , 2 * sizeof(float) });
    attribute_entries.push_back({ VK_FORMAT_R32G32B32_SFLOAT , 3 * sizeof(float) });
    AttributeDescriptor attribute_descriptor(attribute_entries);

    pipeline = std::make_unique<Pipeline>(device);
    pipeline->set_attribute_descriptor(attribute_descriptor);
    pipeline->create(vertex_shader, fragment_shader, *render_pass);
}

void GeometryRenderPass::record_commands(CommandBuffer& command_buffer, uint32_t current_framebuffer) {
    command_buffer.cmd_begin_render_pass(*render_pass, *framebuffers.at(current_framebuffer));
    command_buffer.cmd_bind_pipeline(*pipeline);
    command_buffer.cmd_bind_vertex_buffer(*vertex_buffer);
    command_buffer.cmd_bind_index_buffer(*index_buffer, IndexType::UInt16);
    command_buffer.cmd_set_scissor();
    command_buffer.cmd_set_viewport();
    command_buffer.cmd_draw_indexed(indices.size());
    command_buffer.cmd_end_render_pass();
}