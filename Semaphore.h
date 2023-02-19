#pragma once

#include "Device.h"

class Semaphore {
public:
	Semaphore(Device& device);
	Semaphore(Semaphore &semaphore) = delete;
	~Semaphore();

	VkSemaphore get();

private:
	Device& device;
	VkSemaphore semaphore;
};

