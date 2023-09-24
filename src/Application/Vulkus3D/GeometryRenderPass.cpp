#include "GeometryRenderPass.h"

#include "Logger.h"
#include "Image.h"
#include "Helper.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

GeometryRenderPass::GeometryRenderPass(Device& device, SwapChain& swap_chain, std::vector<SubpassDependency> dependancies) :
    device(device), swap_chain(&swap_chain), sampler(device)
{
    SubpassDependency dependancy;
    dependancy.set_src_subpass(VK_SUBPASS_EXTERNAL);
    dependancy.set_dest_subpass(0);
    dependancy.add_src_stage(PipelineStage::ColourAttachmentOutput | PipelineStage::EarlyFragmentTest); // Only when previous has finished writing...
    dependancy.add_dest_stage(PipelineStage::ColourAttachmentOutput | PipelineStage::EarlyFragmentTest); // will we write the current subpass
    dependancy.add_dest_access(PipelineAccess::ColourAttachmentWrite | PipelineAccess::DepthStencilAttachmentWrite); // Write colour and depth

    attachment_descriptions = {};
    attachment_descriptions.add_attachment(swap_chain.image_format);
    attachment_descriptions.add_attachment(get_supported_depth_format(device.physical_device));

    render_pass = std::make_unique<RenderPass>(device, attachment_descriptions, std::vector{ dependancy });
    pipeline = std::make_unique<Pipeline>(device);
}

void GeometryRenderPass::update_swapchain(SwapChain& swap_chain) {
    this->swap_chain = &swap_chain;
}

void GeometryRenderPass::prepare_framebuffers() {
    framebuffers = std::vector<std::unique_ptr<Framebuffer>>();
    if (swap_chain->images.size() == 0) {
        throw std::runtime_error("Render pass has no targets!");
    }
    for (auto& image : swap_chain->images) {
        std::vector<Image *> attachments{};
        attachments.push_back(&image);
        attachments.push_back(depth_image.get());
        auto framebuffer = std::make_unique<Framebuffer>(device, *render_pass, attachments, *swap_chain);
        Logger::log("Adding framebuffer", Logger::VERBOSE);
        framebuffers.push_back(std::move(framebuffer));
    }
}

void GeometryRenderPass::create_buffers(CommandPool& setup_command_pool, Queue& transfer_queue) {
    VkDeviceSize vertex_data_size = sizeof(vertices[0]) * vertices.size();
    auto vertex_staging_buffer = Buffer::create_buffer(device, vertices, BufferUsage::TransferSource, MemoryProperties::HostVisible | MemoryProperties::HostCoherent);
    vertex_buffer = Buffer::create_empty_buffer(device, vertex_data_size, BufferUsage::TransferDestination | BufferUsage::Vertex, MemoryProperties::DeviceLocal);

    VkDeviceSize index_data_size = sizeof(indices[0]) * indices.size();
    auto index_staging_buffer = Buffer::create_buffer(device, indices, BufferUsage::TransferSource, MemoryProperties::HostVisible | MemoryProperties::HostCoherent);
    index_buffer = Buffer::create_empty_buffer(device, index_data_size, BufferUsage::TransferDestination | BufferUsage::Index, MemoryProperties::DeviceLocal);

    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    auto [texture_buffer, width, height] = Buffer::create_buffer_from_image(device, "assets/textures/texture.jpg", BufferUsage::TransferSource, MemoryProperties::HostVisible | MemoryProperties::HostCoherent);
    Logger::log("Creating colour image");
    image = std::make_unique<Image>(device, format, width, height);

    VkFormat depth_format = get_supported_depth_format(device.physical_device);
    Logger::log("Creating depth image");
    depth_image = std::make_unique<Image>(device, depth_format, swap_chain->get_extent().width, swap_chain->get_extent().height, ImageType::DEPTH);
    Logger::log("Images created!");
    
    CommandBuffer command_buffer(device, setup_command_pool);
    command_buffer.start_recording(true);

    // Copy vertex data
    command_buffer.cmd_copy_buffer(*vertex_staging_buffer, *vertex_buffer, vertex_data_size);

    // Copy index data
    command_buffer.cmd_copy_buffer(*index_staging_buffer, *index_buffer, index_data_size);

    // Transition image
    command_buffer.cmd_image_pipeline_barrier(*image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    command_buffer.cmd_copy_buffer_to_image(*texture_buffer, *image, width, height);
    command_buffer.cmd_image_pipeline_barrier(*image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Transition depth
    command_buffer.cmd_image_pipeline_barrier(*depth_image, depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    command_buffer.stop_recording();

    transfer_queue.submit(command_buffer);
    transfer_queue.wait_idle();
}

void GeometryRenderPass::prepare_pipeline() {
    Shader vertex_shader(device, "Vertices_vert.spv");
    Shader fragment_shader(device, "Vertices_frag.spv");

    pipeline->create(vertex_shader, fragment_shader, *render_pass);
}

void GeometryRenderPass::record_commands(CommandBuffer& command_buffer, uint32_t current_framebuffer, uint32_t current_frame) {
    command_buffer.cmd_begin_render_pass(*render_pass, *framebuffers.at(current_framebuffer), attachment_descriptions);
    command_buffer.cmd_bind_pipeline(*pipeline);
    command_buffer.cmd_bind_vertex_buffer(*vertex_buffer);
    command_buffer.cmd_bind_index_buffer(*index_buffer, IndexType::UInt16);
    command_buffer.cmd_bind_descriptor_set(*descriptor_pool, *pipeline, current_frame);
    command_buffer.cmd_set_scissor();
    command_buffer.cmd_set_viewport();
    command_buffer.cmd_draw_indexed(indices.size());
    command_buffer.cmd_end_render_pass();
}

void GeometryRenderPass::setup_descriptor_sets(uint32_t num_descriptor_sets) {
    descriptor_sets.emplace_back(ShaderStage::Vertex, DescriptorType::Sampler, sizeof(Transformations));
    descriptor_sets.emplace_back(ShaderStage::Fragment, DescriptorType::CombinedImageSampler, 0);

    std::vector<AttributeEntry> attribute_entries;
    attribute_entries.emplace_back(VK_FORMAT_R32G32B32_SFLOAT, 3 * sizeof(float));
    attribute_entries.emplace_back(VK_FORMAT_R32G32B32_SFLOAT, 3 * sizeof(float));
    attribute_entries.emplace_back(VK_FORMAT_R32G32_SFLOAT, 2 * sizeof(float));
    AttributeDescriptor attribute_descriptor(attribute_entries);

    pipeline->set_attribute_descriptor(attribute_descriptor);
    for (uint32_t i = 0; i < descriptor_sets.size(); i++) {
        DescriptorSetInfo descriptor_set = descriptor_sets[i];
        pipeline->add_descriptor_set_binding(i, descriptor_set.shader_stage, get_access_type(descriptor_set.descriptor_type));
    }

    for (size_t i = 0; i < num_descriptor_sets; i++) {
        VkDeviceSize buffer_size = sizeof(Transformations);
        descriptor_set_buffers.push_back(Buffer::create_empty_buffer(device, buffer_size, BufferUsage::Uniform, MemoryProperties::HostVisible | MemoryProperties::HostCoherent, LocalMemory::Persistent));
    }
}

void GeometryRenderPass::prepare_descriptor_sets(uint32_t num_descriptor_sets) {
    if (image == nullptr) {
        throw std::runtime_error("Image hasn't been setup yet");
    }

    std::vector<DescriptorPool::DescriptorAccess> descriptor_accesses{};
    descriptor_accesses.push_back(&descriptor_set_buffers);
    descriptor_accesses.push_back(DescriptorPool::ImageSampler(image->get_view(), sampler.get()));

    descriptor_pool = std::make_unique<DescriptorPool>(device, descriptor_sets, num_descriptor_sets);
    descriptor_pool->allocate_descriptor_set(pipeline->get_descriptor_set_layout());
    descriptor_pool->update_descriptor_sets(descriptor_accesses);
}

void GeometryRenderPass::update_descriptor_sets(uint32_t screen_width, uint32_t screen_height, uint32_t buffer_index) {
    static auto start_time = std::chrono::high_resolution_clock::now();

    auto current_time = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

    const glm::mat4 identity = glm::mat4(1.0f);
    const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 back = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 centre = glm::vec3(0.0f, 0.0f, 0.0f);

    float rotation = time * glm::radians(90.0f);
    glm::vec3 camera_position = glm::vec3(2.0f, 2.0f, 2.0f);
    float fov = glm::radians(45.0f);

    Transformations transformations{};
    transformations.model = glm::rotate(identity, rotation, back);
    transformations.view = glm::lookAt(camera_position, centre, up);
    transformations.projection = glm::perspective(fov, screen_width / (float) screen_height, 0.1f, 10.0f);
    transformations.projection[1][1] *= -1; // Y-coordinate is inverted compared to OpenGL

    descriptor_set_buffers.at(buffer_index).fill_buffer(&transformations, sizeof(Transformations));
}