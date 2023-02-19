#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

#include "Type.h"

std::vector<char> read_file(const std::string& filename);

VkPipelineStageFlags get_pipeline_stage_flags(PipelineStage pipeline_stage);

VkAccessFlagBits get_pipeline_access_flags(PipelineAccess pipeline_access);