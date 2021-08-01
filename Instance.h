#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <memory>
#include <vector>

#include "ValidationLayers.h"

#define MAX_NO_EXTENSIONS 2048

struct Version {
	int major;
	int minor;
	int rev;
};

class Instance {
public:
	std::unique_ptr<VkInstance> instance;

	Instance(
		std::string app_name, Version app_version,
		std::string engine_name, Version engine_version,
		int vulkan_version, std::vector<std::string> required_extensions,
		std::vector<std::string> required_layers
	);
	~Instance();
};

