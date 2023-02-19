#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <algorithm>
#include <set>

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
		const std::string &app_name, const Version &app_version,
		const std::string &engine_name, const Version &engine_version,
		int vulkan_version, Settings &settings,
		const std::set<std::string> &required_extensions,
		const std::set<std::string> &required_layers
	);
	~Instance();

	VkInstance get() const;

private:
	VkInstance instance;
	std::optional<DebugMessenger> debug_messenger;
};

