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

VkPipelineStageFlags get_pipeline_stage_flags(PipelineStage pipeline_stage) {
    switch (pipeline_stage) {
    case ColourAttachmentOutput:
        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    assert(false);
}



VkAccessFlagBits get_pipeline_access_flags(PipelineAccess pipeline_access) {
    switch (pipeline_access) {
    case ColourAttachmentWrite:
        return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    assert(false);
}