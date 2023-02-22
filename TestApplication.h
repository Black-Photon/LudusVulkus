#pragma once

#include "Application.h"
#include "Semaphore.h"
#include "Fence.h"

class TestApplication : public Application {
public:
	static inline const std::string name = "Triangle Engine";
	static inline const Version version = { 1, 0, 0 };

	TestApplication(Instance& instance, Device& device, Window& window, Surface& surface, Settings& settings);
	~TestApplication();

	void prepare() override;
	void update() override;
	void on_close() override;

private:
	std::unique_ptr<Semaphore> image_available;
	std::unique_ptr<Semaphore> render_finished;
	std::unique_ptr<Fence> image_in_flight;
};

