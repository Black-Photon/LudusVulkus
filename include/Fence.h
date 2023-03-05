#pragma once

#include "Device.h"

class Fence {
public:
	Fence(Device& device);
	~Fence();

	VkFence get();

	void wait(uint64_t timeout = UINT64_MAX);
	void reset();

	static void wait_all(std::vector<Fence> &fences, uint64_t timeout = UINT64_MAX);
	static void wait_any(std::vector<Fence>& fences, uint64_t timeout = UINT64_MAX);

private:
	Device &device;
	VkFence fence;

	static void wait_internal(std::vector<Fence>& fences, uint64_t timeout, VkBool32 wait_all);
};

