#include "Pipeline.h"

#include "Logger.h"

Pipeline::Pipeline(Device& device) :
	device(device)
{
}

Pipeline::~Pipeline() {
	if (!setup) return;
	Logger::log("Freeing Pipeline", Logger::VERBOSE);
	vkDestroyPipelineLayout(device.get(), pipeline_layout, nullptr);
	vkDestroyPipeline(device.get(), pipeline, nullptr);
	if (descriptor_set_layout.has_value()) vkDestroyDescriptorSetLayout(device.get(), descriptor_set_layout.value(), nullptr);
}

void Pipeline::create(Shader& vertex_shader, Shader& fragment_shader, RenderPass &render_pass) {
	VkPipelineShaderStageCreateInfo vertex_stage_info = create_shader_stage(vertex_shader, VERTEX);
	VkPipelineShaderStageCreateInfo fragment_stage_info = create_shader_stage(fragment_shader, FRAGMENT);
	VkPipelineShaderStageCreateInfo shader_stage_infos[] = { vertex_stage_info, fragment_stage_info };

	DynamicState dynamic_state(true, true);
	VkPipelineDynamicStateCreateInfo dynamic_state_info = create_dynamic_state(dynamic_state);

	VkPipelineVertexInputStateCreateInfo vertex_input_info = create_vertex_input_state();
	VkPipelineInputAssemblyStateCreateInfo input_assembly_info = create_input_assembly_state();
	VkPipelineViewportStateCreateInfo viewport_info = create_viewport_state(dynamic_state);
	VkPipelineRasterizationStateCreateInfo rasterization_info = create_rasterization_state();
	VkPipelineMultisampleStateCreateInfo multisample_info = create_multisample_state();
	VkPipelineDepthStencilStateCreateInfo depth_stencil_info = create_depth_stencil_state();

	VkPipelineColorBlendAttachmentState color_blend_attachement_info = create_color_blend_attachment_state();
	std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_infos;
	color_blend_attachment_infos.push_back(color_blend_attachement_info);

	VkPipelineColorBlendStateCreateInfo color_blend_info = create_color_blend_state(color_blend_attachment_infos);

	create_descriptor_set_layout();

	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	if (descriptor_set_layout.has_value()) {
		pipeline_layout_info.setLayoutCount = 1;
		pipeline_layout_info.pSetLayouts = &descriptor_set_layout.value();
	} else {
		pipeline_layout_info.setLayoutCount = 0;
		pipeline_layout_info.pSetLayouts = nullptr;
	}
	pipeline_layout_info.pushConstantRangeCount = 0;
	pipeline_layout_info.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(device.get(), &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
		throw std::runtime_error("Could not create pipeline layout");
	}

	VkGraphicsPipelineCreateInfo pipeline_info{};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shader_stage_infos;
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly_info;
	pipeline_info.pViewportState = &viewport_info;
	pipeline_info.pRasterizationState = &rasterization_info;
	pipeline_info.pMultisampleState = &multisample_info;
	pipeline_info.pDepthStencilState = nullptr;
	pipeline_info.pColorBlendState = &color_blend_info;
	pipeline_info.pDynamicState = &dynamic_state_info;
	pipeline_info.layout = pipeline_layout;
	pipeline_info.renderPass = render_pass.get();
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_info.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device.get(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline");
	}

	setup = true;
}

void Pipeline::set_attribute_descriptor(AttributeDescriptor attribute_descriptor) {
	this->attribute_descriptor = attribute_descriptor;
}

void Pipeline::add_descriptor_set_binding(uint32_t binding, VkShaderStageFlags shader_stages, VkDescriptorType descriptor_type) {
	VkDescriptorSetLayoutBinding descriptor_set_binding{};
	descriptor_set_binding.binding = binding;
	descriptor_set_binding.descriptorType = descriptor_type;
	descriptor_set_binding.descriptorCount = 1;
	descriptor_set_binding.stageFlags = shader_stages;
	descriptor_set_binding.pImmutableSamplers = nullptr;

	descriptor_set_bindings.push_back(descriptor_set_binding);
}

VkPipeline Pipeline::get() {
	if (!setup) {
		throw std::runtime_error("Pipeline has not been setup with create()");
	}
	return pipeline;
}

VkPipelineLayout Pipeline::get_layout() {
	if (!setup) {
		throw std::runtime_error("Pipeline has not been setup with create()");
	}
	return pipeline_layout;
}

VkDescriptorSetLayout Pipeline::get_descriptor_set_layout() {
	if (!setup) {
		throw std::runtime_error("Pipeline has not been setup with create()");
	}
	if (!descriptor_set_layout.has_value()) {
		throw std::runtime_error("This pipeline doesn't have any descriptor set bindings");
	}
	return descriptor_set_layout.value();
}

VkPipelineShaderStageCreateInfo Pipeline::create_shader_stage(Shader& shader, ShaderType type) {
	VkPipelineShaderStageCreateInfo shader_stage_info{};
	shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	switch (type) {
	case VERTEX:
		shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case FRAGMENT:
		shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	default:
		throw std::runtime_error("Unknown shader stage");
	}
	shader_stage_info.module = shader.get();
	shader_stage_info.pName = "main";
	return shader_stage_info;
}

VkPipelineDynamicStateCreateInfo Pipeline::create_dynamic_state(DynamicState& dynamic_state) {
	VkPipelineDynamicStateCreateInfo dynamic_state_info{};
	dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(dynamic_state.dynamic_states.size());
	dynamic_state_info.pDynamicStates = dynamic_state.dynamic_states.data();
	return dynamic_state_info;
}

VkPipelineVertexInputStateCreateInfo Pipeline::create_vertex_input_state() {
	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	if (attribute_descriptor.has_value()) {
		vertex_input_info.vertexBindingDescriptionCount = 1;
		vertex_input_info.pVertexBindingDescriptions = &attribute_descriptor->binding_descriptor;
		vertex_input_info.vertexAttributeDescriptionCount = attribute_descriptor->attribute_descriptors.size();
		vertex_input_info.pVertexAttributeDescriptions = attribute_descriptor->attribute_descriptors.data();
	} else {
		vertex_input_info.vertexBindingDescriptionCount = 0;
		vertex_input_info.pVertexBindingDescriptions = nullptr;
		vertex_input_info.vertexAttributeDescriptionCount = 0;
		vertex_input_info.pVertexAttributeDescriptions = nullptr;
	}
	return vertex_input_info;
}

VkPipelineInputAssemblyStateCreateInfo Pipeline::create_input_assembly_state() {
	VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
	input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_info.primitiveRestartEnable = VK_FALSE;
	return input_assembly_info;
}

VkPipelineViewportStateCreateInfo Pipeline::create_viewport_state(DynamicState &dynamic_state) {
	VkPipelineViewportStateCreateInfo viewport_state{};
	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	if (!dynamic_state.is_viewport_dynamic()) {
		throw std::runtime_error("Static viewport is not set up during pipeline setup");
	}
	viewport_state.viewportCount = 1;
	if (!dynamic_state.is_scissor_dynamic()) {
		throw std::runtime_error("Static scissor is not set up during pipeline setup");
	}
	viewport_state.scissorCount = 1;
	return viewport_state;
}

VkPipelineRasterizationStateCreateInfo Pipeline::create_rasterization_state() {
	VkPipelineRasterizationStateCreateInfo rasterizer_info{};
	rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer_info.depthClampEnable = VK_FALSE;
	rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
	rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer_info.lineWidth = 1.0f;
	rasterizer_info.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer_info.depthBiasEnable = VK_FALSE;
	rasterizer_info.depthBiasConstantFactor = 0.0f;
	rasterizer_info.depthBiasClamp = 0.0f;
	rasterizer_info.depthBiasSlopeFactor = 0.0f;
	return rasterizer_info;
}

VkPipelineMultisampleStateCreateInfo Pipeline::create_multisample_state() {
	VkPipelineMultisampleStateCreateInfo multisampling_info{};
	multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling_info.sampleShadingEnable = VK_FALSE;
	multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling_info.minSampleShading = 1.0f;
	multisampling_info.pSampleMask = nullptr;
	multisampling_info.alphaToCoverageEnable = VK_FALSE;
	multisampling_info.alphaToOneEnable = VK_FALSE;
	return multisampling_info;
}

VkPipelineColorBlendAttachmentState Pipeline::create_color_blend_attachment_state() {
	VkPipelineColorBlendAttachmentState blend_attachement_info{};
	blend_attachement_info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	blend_attachement_info.blendEnable = VK_FALSE;
	blend_attachement_info.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	blend_attachement_info.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	blend_attachement_info.colorBlendOp = VK_BLEND_OP_ADD;
	blend_attachement_info.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	blend_attachement_info.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	blend_attachement_info.alphaBlendOp = VK_BLEND_OP_ADD;
	return blend_attachement_info;
}

VkPipelineColorBlendStateCreateInfo Pipeline::create_color_blend_state(std::vector<VkPipelineColorBlendAttachmentState> &blend_attachment_infos) {
	VkPipelineColorBlendStateCreateInfo color_blend_info{};
	color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_info.logicOpEnable = VK_FALSE;
	color_blend_info.logicOp = VK_LOGIC_OP_COPY;
	color_blend_info.attachmentCount = blend_attachment_infos.size();
	color_blend_info.pAttachments = blend_attachment_infos.data();
	color_blend_info.blendConstants[0] = 0.0f;
	color_blend_info.blendConstants[1] = 0.0f;
	color_blend_info.blendConstants[2] = 0.0f;
	color_blend_info.blendConstants[3] = 0.0f;
	return color_blend_info;
}

VkPipelineDepthStencilStateCreateInfo Pipeline::create_depth_stencil_state() {
	VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
	depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_info.depthTestEnable = VK_TRUE;
	depth_stencil_info.depthWriteEnable = VK_TRUE;
	depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_info.stencilTestEnable = VK_FALSE;
	return depth_stencil_info;
}

void Pipeline::create_descriptor_set_layout() {
	if (descriptor_set_bindings.size() == 0) return;

	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
	descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_info.bindingCount = descriptor_set_bindings.size();
	descriptor_set_layout_info.pBindings = descriptor_set_bindings.data();

	VkDescriptorSetLayout descriptor_set_layout;
	if (vkCreateDescriptorSetLayout(device.get(), &descriptor_set_layout_info, nullptr, &descriptor_set_layout) != VK_SUCCESS) {
		throw std::runtime_error("Unable to create descriptor set layout");
	}

	this->descriptor_set_layout = descriptor_set_layout;
}