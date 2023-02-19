#pragma once

#include <string>

#include "Device.h"

class Shader {
public:
	Shader(Device &device, std::string filename);
	~Shader();

	VkShaderModule get();

private:
	Device &device;
	VkShaderModule shader_module;
};

