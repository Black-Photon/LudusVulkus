#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

#include "PhysicalDevice.h"
#include "Type.h"

static std::vector<VkFormat> depth_formats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
static std::vector<VkFormat> stencil_formats = { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };

std::vector<char> read_file(const std::string& filename);

std::string bool_str(bool b);

VkDescriptorType get_access_type(VkDescriptorType descriptor_type);

VkFormat get_supported_depth_format(PhysicalDevice device);

bool has_depth(VkFormat format);

bool has_stencil(VkFormat format);