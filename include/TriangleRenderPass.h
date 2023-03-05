#pragma once
#include "RenderPass.h"

#include <glm/glm.hpp>

#include "Pipeline.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "SwapChain.h"
#include "CommandBuffer.h"

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

class TriangleRenderPass {
public:
	TriangleRenderPass(Device& device, SwapChain &swap_chain, std::vector<SubpassDependency> dependancies = {});
	void update_swapchain(SwapChain& swap_chain);
	void prepare_framebuffers();
	void prepare_pipeline();
	void record_commands(CommandBuffer &command_buffer, uint32_t current_framebuffer);

private:
	Device& device;
	std::unique_ptr<RenderPass> render_pass;
	SwapChain *swap_chain;
	std::vector<std::unique_ptr<Framebuffer>> framebuffers;
	std::unique_ptr<Pipeline> pipeline;
};
