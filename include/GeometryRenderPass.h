#pragma once
#include "RenderPass.h"

#include <glm/glm.hpp>

#include "Pipeline.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "SwapChain.h"
#include "CommandBuffer.h"
#include "Queue.h"
#include "DescriptorSetInfo.h"
#include "Sampler.h"

class GeometryRenderPass {
public:
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;
		glm::vec2 tex_coord;
	};

	GeometryRenderPass(Device& device, SwapChain& swap_chain, std::vector<SubpassDependency> dependancies = {});
	void update_swapchain(SwapChain& swap_chain);
	void prepare_framebuffers();
	void create_buffers(CommandPool& setup_command_pool, Queue& transfer_queue);
	void prepare_pipeline();
	void record_commands(CommandBuffer& command_buffer, uint32_t current_framebuffer, uint32_t current_frame);
	void setup_descriptor_sets(uint32_t num_descriptor_sets);
	void prepare_descriptor_sets(uint32_t num_descriptor_sets);
	void update_descriptor_sets(uint32_t screen_width, uint32_t screen_height, uint32_t buffer_index);

private:
	struct Transformations {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	} transformations;

	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	Device& device;
	Sampler sampler;
	std::unique_ptr<RenderPass> render_pass;
	SwapChain* swap_chain;
	std::vector<std::unique_ptr<Framebuffer>> framebuffers;
	std::unique_ptr<Pipeline> pipeline;
	std::unique_ptr<Buffer> vertex_buffer;
	std::unique_ptr<Buffer> index_buffer;

	std::unique_ptr<DescriptorPool> descriptor_pool;
	boost::ptr_vector<Buffer> descriptor_set_buffers{};
	std::unique_ptr<Image> image;
	std::vector<DescriptorSetInfo> descriptor_sets;
};

