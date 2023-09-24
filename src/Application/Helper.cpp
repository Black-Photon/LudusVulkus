#include "Helper.h"

#include <fstream>
#include <cassert>

std::vector<char> read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open the file at " + filename);
    }

    // Start reading at end (from ios::ate) to find file size
    size_t file_size = (size_t) file.tellg();
    std::vector<char> buffer(file_size);

    // Move back to start
    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();

    return buffer;
}

std::string bool_str(bool b) {
    return b ? "true" : "false";
}

VkDescriptorType get_access_type(VkDescriptorType descriptor_type) {
    switch (descriptor_type) {
    case DescriptorType::Sampler:
        return DescriptorType::UniformBuffer;
    case DescriptorType::CombinedImageSampler:
        return DescriptorType::CombinedImageSampler;
    default:
        throw std::runtime_error("Unknown descriptor type with index " + std::to_string(descriptor_type));
    }
}

VkFormat get_supported_depth_format(PhysicalDevice device) {
    return device.first_supported_format(
        depth_formats,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool has_depth(VkFormat format) {
    for (VkFormat depth_format : depth_formats) {
        if (format == depth_format) return true;
    }
    return false;
}

bool has_stencil(VkFormat format) {
    for (VkFormat stencil_format : stencil_formats) {
        if (format == stencil_format) return true;
    }
    return false;
}