#include "Shader.h"

#include <vulkan/vulkan.h>
#include <stdexcept>

#include "Helper.h"
#include "Logger.h"

Shader::Shader(Device &device, std::string filename) :
	device(device)
{
	std::vector<char> shader_code = read_file(filename);

	VkShaderModuleCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = shader_code.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>(shader_code.data());

	if (vkCreateShaderModule(device.get(), &create_info, nullptr, &shader_module) != VK_SUCCESS) {
		throw std::runtime_error("Could not create shader module");
	}
}

Shader::~Shader() {
	Logger::log("Freeing Shader", Logger::VERBOSE);
	vkDestroyShaderModule(device.get(), shader_module, nullptr);
}

VkShaderModule Shader::get() {
	return shader_module;
}