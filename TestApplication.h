#pragma once

#include "Application.h"
#include "Semaphore.h"
#include "Fence.h"

class TestApplication : public Application {
public:
	TestApplication();
	~TestApplication();

	void prepare(Window& window) override;
	void update() override;

private:
	std::unique_ptr<Semaphore> image_available;
	std::unique_ptr<Semaphore> render_finished;
	std::unique_ptr<Fence> image_in_flight;
};

