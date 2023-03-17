#pragma once

#include <array>

#include "Application.h"
#include "Semaphore.h"
#include "Fence.h"
#include "GeometryRenderPass.h"

#define FRAMES_IN_FLIGHT 2

class Vulkus3D : public Application {
public:
	struct Frame {
		CommandBuffer& command_buffer;
		std::unique_ptr<Semaphore> image_available;
		std::unique_ptr<Semaphore> render_finished;
		std::unique_ptr<Fence> image_in_flight;
	};

	static inline const std::string name = "Vulkus3D";
	static inline const Version version = { 1, 0, 0 };

	Vulkus3D(Instance& instance, Device& device, Window& window, Surface& surface, Settings& settings);
	~Vulkus3D();

	void prepare() override;
	void update() override;
	void recreate_swapchain() override;
	void on_close() override;

private:
	uint32_t current_frame = 0;
	std::array<std::unique_ptr<Frame>, FRAMES_IN_FLIGHT> frames;
	std::unique_ptr<GeometryRenderPass> render_pass;
};

