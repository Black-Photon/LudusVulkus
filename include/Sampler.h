#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.h"

class Sampler {
public:
	Sampler(const Device &device);
	Sampler(const Sampler&) = delete;
	~Sampler();

	VkSampler get() const;

private:
	const Device& device;
	VkSampler sampler;
};