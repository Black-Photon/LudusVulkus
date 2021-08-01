#include "ValidationLayers.h"

ValidationLayers::ValidationLayers() {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
    layers = available_layers;
}

std::vector<VkLayerProperties> ValidationLayers::get_available_layers() {
    return layers;
}

bool ValidationLayers::is_layer_available(std::string layer) {
    for (auto available_layer : layers) {
        if (layer == available_layer.layerName) return true;
    }
    return false;
}