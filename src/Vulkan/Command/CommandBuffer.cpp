#include "CommandBuffer.h"

#include <cassert>

#include "CommandPool.h"
#include "Logger.h"

CommandBuffer::CommandBuffer(Device &device, CommandPool &command_pool) :
    device(device), command_pool(command_pool)
{
    VkCommandBufferAllocateInfo allocation_info{};
    allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocation_info.commandPool = command_pool.get();
    allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocation_info.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device.get(), &allocation_info, &command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Unable to allocate command buffers");
    }
}

CommandBuffer::~CommandBuffer() {
    Logger::log("Freeing Command Buffer", Logger::VERBOSE);
    vkFreeCommandBuffers(device.get(), command_pool.get(), 1, &command_buffer);
}

VkCommandBuffer CommandBuffer::get() {
    return command_buffer;
}

void CommandBuffer::start_recording(bool one_time) {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = one_time ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;
    begin_info.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Unable to start recording command buffer");
    }
}

void CommandBuffer::cmd_begin_render_pass(RenderPass &render_pass, Framebuffer &framebuffer) {
    VkRenderPassBeginInfo render_pass_begin_info{};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = render_pass.get();
    render_pass_begin_info.framebuffer = framebuffer.get();
    render_pass_begin_info.renderArea.offset = { 0, 0 };
    render_pass_begin_info.renderArea.extent = framebuffer.extent;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clearColor;
    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    this->framebuffer.emplace(&framebuffer);
}

void CommandBuffer::cmd_bind_pipeline(Pipeline& pipeline) {
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get());
}

void CommandBuffer::cmd_bind_vertex_buffer(Buffer &buffer) {
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &buffer.get(), offsets);
}

void CommandBuffer::cmd_bind_index_buffer(Buffer& buffer, VkIndexType index_type) {
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindIndexBuffer(command_buffer, buffer.get(), 0, index_type);
}

void CommandBuffer::cmd_bind_descriptor_set(DescriptorPool& descriptor_pool, Pipeline &pipeline, uint32_t descriptor_index) {
    VkDescriptorSet descriptor_set = descriptor_pool.get_descriptor_set(descriptor_index);
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get_layout(), 0, 1, &descriptor_set, 0, nullptr);
}

/**
 * Creates a default viewport that covers the whole framebuffer
 */
void CommandBuffer::cmd_set_viewport() {
    assert(framebuffer.has_value());
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(framebuffer.value()->extent.width);
    viewport.height = static_cast<float>(framebuffer.value()->extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    cmd_set_viewport(viewport);
}

void CommandBuffer::cmd_set_viewport(VkViewport viewport) {
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
}

/**
 * Creates a default scissor that covers the whole framebuffer
 */
void CommandBuffer::cmd_set_scissor() {
    assert(framebuffer.has_value());
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = framebuffer.value()->extent;
    cmd_set_scissor(scissor);
}

void CommandBuffer::cmd_set_scissor(VkRect2D scissor) {
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void CommandBuffer::cmd_draw(size_t indices) {
    vkCmdDraw(command_buffer, static_cast<uint32_t>(indices), 1, 0, 0);
}

void CommandBuffer::cmd_draw_indexed(size_t indices) {
    vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(indices), 1, 0, 0, 0);
}

void CommandBuffer::cmd_end_render_pass() {
    vkCmdEndRenderPass(command_buffer);
}

void CommandBuffer::cmd_copy_buffer(Buffer& src_buffer, Buffer& dest_buffer, size_t data_size) {
    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0; // Optional
    copy_region.dstOffset = 0; // Optional
    copy_region.size = data_size;
    vkCmdCopyBuffer(command_buffer, src_buffer.get(), dest_buffer.get(), 1, &copy_region);
}

void CommandBuffer::cmd_image_pipeline_barrier(const Image &image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout) {
    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image.get();
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0; // TODO
    barrier.dstAccessMask = 0; // TODO

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("Unsupported layout transition");
    }
    
    vkCmdPipelineBarrier(
        command_buffer,
        source_stage, destination_stage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
}

void CommandBuffer::cmd_copy_buffer_to_image(const Buffer &buffer, const Image &image, uint32_t width, uint32_t height) {
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        command_buffer,
        buffer.get(),
        image.get(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
}

void CommandBuffer::stop_recording() {
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Unable to record command buffer");
    }
}

void CommandBuffer::reset() {
    vkResetCommandBuffer(command_buffer, 0);
}