#pragma once

#include "Shader.h"
#include "DynamicState.h"
#include "SwapChain.h"
#include "RenderPass.h"

enum ShaderType {
	VERTEX, FRAGMENT
};

class Pipeline {
public:
	Pipeline(Shader &vertex_shader, Shader &fragment_shader, Device &device, RenderPass& render_pass);
	~Pipeline();

	VkPipeline get();
	VkPipelineLayout get_layout();
	VkPipelineShaderStageCreateInfo create_shader_stage(Shader& shader, ShaderType type);
	VkPipelineDynamicStateCreateInfo create_dynamic_state(DynamicState& dynamic_state);
	VkPipelineVertexInputStateCreateInfo create_vertex_input_state();
	VkPipelineInputAssemblyStateCreateInfo create_input_assembly_state();
	VkPipelineViewportStateCreateInfo create_viewport_state(DynamicState& dynamic_state);
	VkPipelineRasterizationStateCreateInfo create_rasterization_state();
	VkPipelineMultisampleStateCreateInfo create_multisample_state();
	VkPipelineColorBlendAttachmentState create_color_blend_attachment_state();
	VkPipelineColorBlendStateCreateInfo create_color_blend_state(std::vector<VkPipelineColorBlendAttachmentState> &blend_attachment_infos);

private:
	Device &device;
	VkPipelineLayout pipeline_layout;
	VkPipeline pipeline;
};

