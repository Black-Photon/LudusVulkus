#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"
#include "RenderPass.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "Buffer.h"
#include "DescriptorPool.h"

class CommandPool;

class CommandBuffer {
public:
	CommandBuffer(Device &device, CommandPool &command_pool);
	CommandBuffer(const CommandBuffer&) = delete;
	~CommandBuffer();

	VkCommandBuffer get();

	void start_recording(bool one_time = false);
	void cmd_begin_render_pass(RenderPass& render_pass, Framebuffer &framebuffer);
	void cmd_bind_pipeline(Pipeline &pipeline);
	void cmd_bind_vertex_buffer(Buffer &buffer);
	void cmd_bind_index_buffer(Buffer& buffer, VkIndexType index_type);
	void cmd_bind_descriptor_set(DescriptorPool& descriptor_pool, Pipeline& pipeline, uint32_t descriptor_index);
	void cmd_set_viewport();
	void cmd_set_viewport(VkViewport viewport);
	void cmd_set_scissor();
	void cmd_set_scissor(VkRect2D scissor);
	void cmd_draw(size_t indices);
	void cmd_draw_indexed(size_t indices);
	void cmd_end_render_pass();
	void cmd_copy_buffer(Buffer& src_buffer, Buffer& dest_buffer, size_t data_size);
	void stop_recording();

	void reset();

private:
	Device &device;
	CommandPool &command_pool;
	VkCommandBuffer command_buffer;
	std::optional<Framebuffer *> framebuffer;
};

