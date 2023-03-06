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

void CommandBuffer::start_recording() {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
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

void CommandBuffer::cmd_end_render_pass() {
    vkCmdEndRenderPass(command_buffer);
}

void CommandBuffer::stop_recording() {
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Unable to record command buffer");
    }
}

void CommandBuffer::reset() {
    vkResetCommandBuffer(command_buffer, 0);
}