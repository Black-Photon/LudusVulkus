#include "Sampler.h"

Sampler::Sampler(const Device& device) : device(device) {
	VkPhysicalDeviceProperties properties = device.physical_device.device_properties;
	VkPhysicalDeviceFeatures features = device.physical_device.device_features;

	VkSamplerCreateInfo sampler_info{};
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter = VK_FILTER_LINEAR;
	sampler_info.minFilter = VK_FILTER_LINEAR;
	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.anisotropyEnable = features.samplerAnisotropy;
	sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
	sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.mipLodBias = 0.0f;
	sampler_info.minLod = 0.0f;
	sampler_info.maxLod = 0.0f;

	if (vkCreateSampler(device.get(), &sampler_info, nullptr, &sampler) != VK_SUCCESS) {
		throw std::runtime_error("Unable to create sampler");
	}
}

Sampler::~Sampler() {
	vkDestroySampler(device.get(), sampler, nullptr);
}

VkSampler Sampler::get() const {
	return sampler;
}