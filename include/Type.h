#pragma once

#include <vulkan/vulkan.h>
#include <cassert>

namespace PipelineStage {
    constexpr VkPipelineStageFlags TopOfPipe = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    constexpr VkPipelineStageFlags DrawIndirect = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
    constexpr VkPipelineStageFlags VertexInput = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    constexpr VkPipelineStageFlags VertexShader = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    constexpr VkPipelineStageFlags TesselationControlShader = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
    constexpr VkPipelineStageFlags TesselationEvalutionShader = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
    constexpr VkPipelineStageFlags GeometryShader = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
    constexpr VkPipelineStageFlags FragmentShader = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    constexpr VkPipelineStageFlags EarlyFragmentTest = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    constexpr VkPipelineStageFlags LateFragmentTest = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    constexpr VkPipelineStageFlags ColourAttachmentOutput = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    constexpr VkPipelineStageFlags ComputeShader = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    constexpr VkPipelineStageFlags TransferBit = VK_PIPELINE_STAGE_TRANSFER_BIT;
    constexpr VkPipelineStageFlags BottomOfPipe = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    constexpr VkPipelineStageFlags Host = VK_PIPELINE_STAGE_HOST_BIT;
    constexpr VkPipelineStageFlags AllGraphics = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
    constexpr VkPipelineStageFlags AllCommands = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
}

namespace PipelineAccess {
    constexpr VkAccessFlags IndirectCommandRead = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    constexpr VkAccessFlags IndexRead = VK_ACCESS_INDEX_READ_BIT;
    constexpr VkAccessFlags VertexAttributeRead = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    constexpr VkAccessFlags UniformRead = VK_ACCESS_UNIFORM_READ_BIT;
    constexpr VkAccessFlags InputAttachmentRead = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    constexpr VkAccessFlags ShaderRead = VK_ACCESS_SHADER_READ_BIT;
    constexpr VkAccessFlags ShaderWrite = VK_ACCESS_SHADER_WRITE_BIT;
    constexpr VkAccessFlags ColourAttachmentRead = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    constexpr VkAccessFlags ColourAttachmentWrite = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    constexpr VkAccessFlags DepthStencilAttachmentRead = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    constexpr VkAccessFlags DepthStencilAttachmentWrite = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    constexpr VkAccessFlags TransferRead = VK_ACCESS_TRANSFER_READ_BIT;
    constexpr VkAccessFlags TransferWrite = VK_ACCESS_TRANSFER_WRITE_BIT;
    constexpr VkAccessFlags HostRead = VK_ACCESS_HOST_READ_BIT;
    constexpr VkAccessFlags HostWrite = VK_ACCESS_HOST_WRITE_BIT;
    constexpr VkAccessFlags MemoryRead = VK_ACCESS_MEMORY_READ_BIT;
    constexpr VkAccessFlags MemoryWrite = VK_ACCESS_MEMORY_WRITE_BIT;
}

namespace BufferUsage {
    constexpr VkBufferUsageFlags TransferSource = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    constexpr VkBufferUsageFlags TransferDestination = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    constexpr VkBufferUsageFlags UniformTexel = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
    constexpr VkBufferUsageFlags StorageTexel = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
    constexpr VkBufferUsageFlags Uniform = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    constexpr VkBufferUsageFlags Storage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    constexpr VkBufferUsageFlags Index = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    constexpr VkBufferUsageFlags Vertex = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    constexpr VkBufferUsageFlags Indirect = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
}

namespace MemoryProperties {
    constexpr VkMemoryPropertyFlags DeviceLocal = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    constexpr VkMemoryPropertyFlags HostVisible = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    constexpr VkMemoryPropertyFlags HostCoherent = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    constexpr VkMemoryPropertyFlags HostCached = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    constexpr VkMemoryPropertyFlags LazilyAllocated = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
}

namespace IndexType {
    constexpr VkIndexType UInt16 = VK_INDEX_TYPE_UINT16;
    constexpr VkIndexType UInt32 = VK_INDEX_TYPE_UINT32;
}

namespace ShaderStage {
    constexpr VkShaderStageFlags Vertex = VK_SHADER_STAGE_VERTEX_BIT;
    constexpr VkShaderStageFlags TessellationControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    constexpr VkShaderStageFlags TessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    constexpr VkShaderStageFlags Geometry = VK_SHADER_STAGE_GEOMETRY_BIT;
    constexpr VkShaderStageFlags Fragment = VK_SHADER_STAGE_FRAGMENT_BIT;
    constexpr VkShaderStageFlags Compute = VK_SHADER_STAGE_COMPUTE_BIT;
    constexpr VkShaderStageFlags AllGraphics = VK_SHADER_STAGE_ALL_GRAPHICS;
    constexpr VkShaderStageFlags All = VK_SHADER_STAGE_ALL;
}

typedef uint32_t LocalMemoryAllocation;

namespace LocalMemory {
    constexpr LocalMemoryAllocation Dynamic = 0;    // Memory is mapped when being set and will be unmapped immediately after transferring data
    constexpr LocalMemoryAllocation Persistent = 1; // Memory is mapped when the buffer is first created and unmapped when the buffer is destroyed
}