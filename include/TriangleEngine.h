#pragma once

#include <array>

#include "Application.h"
#include "Semaphore.h"
#include "Fence.h"
#include "TriangleRenderPass.h"

#define FRAMES_IN_FLIGHT 2

struct Frame {
	CommandBuffer& command_buffer;
	std::unique_ptr<Semaphore> image_available;
	std::unique_ptr<Semaphore> render_finished;
	std::unique_ptr<Fence> image_in_flight;
};

class TriangleEngine : public Application {
public:
	static inline const std::string name = "Triangle Engine";
	static inline const Version version = { 1, 0, 0 };

	TriangleEngine(Instance& instance, Device& device, Window& window, Surface& surface, Settings& settings);
	~TriangleEngine();

	void prepare() override;
	void update() override;
	void recreate_swapchain() override;
	void on_close() override;

private:
	uint32_t current_frame = 0;
	std::array<std::unique_ptr<Frame>, FRAMES_IN_FLIGHT> frames;
	std::unique_ptr<TriangleRenderPass> render_pass;
};

