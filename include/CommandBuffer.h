#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"
#include "RenderPass.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "Buffer.h"

class CommandPool;

class CommandBuffer {
public:
	CommandBuffer(Device &device, CommandPool &command_pool);
	CommandBuffer(const CommandBuffer&) = delete;
	~CommandBuffer();

	VkCommandBuffer get();

	void start_recording();
	void cmd_begin_render_pass(RenderPass& render_pass, Framebuffer &framebuffer);
	void cmd_bind_pipeline(Pipeline &pipeline);
	void cmd_bind_vertex_buffer(Buffer &buffer);
	void cmd_set_viewport();
	void cmd_set_viewport(VkViewport viewport);
	void cmd_set_scissor();
	void cmd_set_scissor(VkRect2D scissor);
	void cmd_draw(size_t indices);
	void cmd_end_render_pass();
	void stop_recording();

	void reset();

private:
	Device &device;
	CommandPool &command_pool;
	VkCommandBuffer command_buffer;
	std::optional<Framebuffer *> framebuffer;
};

