#pragma once

#include <string>

#include "Device.h"

class Shader {
public:
	Shader(std::string filename, std::shared_ptr<Device> device);
	~Shader();

	VkShaderModule get();

private:
	std::shared_ptr<Device> device;
	VkShaderModule shader_module;
};

