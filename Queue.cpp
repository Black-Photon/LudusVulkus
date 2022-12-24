#include "Queue.h"

#include "Device.h"

Queue::Queue(QueueFamily &queue_family) : queue_family(queue_family) {
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = queue_family.index;
	queue_create_info.queueCount = 1;
	queue_create_info.pQueuePriorities = &priority;
}

void Queue::setup_queue(Device &device) {
	VkQueue queue;
	vkGetDeviceQueue(device.get(), queue_family.index, 0, &queue);
	this->queue = queue;
}

VkQueue& Queue::get() {
	return queue.value();
}