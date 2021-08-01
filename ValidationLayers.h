#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class ValidationLayers {
public:
	ValidationLayers();
	std::vector<VkLayerProperties> get_available_layers();
	bool is_layer_available(std::string layer);

private:
	std::vector<VkLayerProperties> layers;
};

