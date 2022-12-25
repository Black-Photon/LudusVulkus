#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <algorithm>

#include "ValidationLayers.h"
#include "DebugMessenger.h"
#include "Settings.h"

#define MAX_NO_EXTENSIONS 2048

struct Version {
	int major;
	int minor;
	int rev;
};

class Instance {
public:
	Instance(
		std::string app_name, Version app_version,
		std::string engine_name, Version engine_version,
		int vulkan_version, std::shared_ptr<Settings> settings,
		std::vector<std::string> required_extensions,
		std::vector<std::string> required_layers
	);
	~Instance();

	VkInstance get();

private:
	VkInstance instance;
	std::optional<DebugMessenger> debug_messenger;
};

