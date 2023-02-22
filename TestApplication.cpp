#include "TestApplication.h"

#include "Logger.h"

TestApplication::TestApplication(Instance& instance, Device& device, Window& window, Surface& surface, Settings& settings) :
	Application(instance, device, window, surface, settings)
{
}

TestApplication::~TestApplication() {
	Logger::log("Deleting test app");
}

void TestApplication::prepare() {
	Application::prepare();

	image_available = std::make_unique<Semaphore>(*device);
	render_finished = std::make_unique<Semaphore>(*device);
	image_in_flight = std::make_unique<Fence>(*device);
}

void TestApplication::update() {
	Application::update();

	CommandBuffer& command_buffer = *command_pool->command_buffers.at(0);
	Queue& graphics_queue = *device->queues.at(GRAPHICS);
	Queue& present_queue = *device->queues.at(PRESENT);

	image_in_flight->wait();
	image_in_flight->reset();

	ImageIndex image_index = swap_chain->get_next_image(*image_available);

	command_buffer.reset();
	command_buffer.start_recording();

	command_buffer.cmd_begin_render_pass(*render_pass, *framebuffers.at(image_index));
	command_buffer.cmd_bind_pipeline(*pipeline);
	command_buffer.cmd_set_scissor();
	command_buffer.cmd_set_viewport();
	command_buffer.cmd_draw();
	command_buffer.cmd_end_render_pass();

	command_buffer.stop_recording();

	auto wait_semaphores = std::vector<std::pair<Semaphore *, PipelineStage>>();
	wait_semaphores.push_back(std::pair(image_available.get(), ColourAttachmentOutput));

	auto signal_semaphores = std::vector<Semaphore *>();
	signal_semaphores.push_back(render_finished.get());

	graphics_queue.submit(command_buffer, wait_semaphores, signal_semaphores, image_in_flight.get());

	present_queue.present(*swap_chain, image_index, signal_semaphores);
}

void TestApplication::on_close() {
	Application::on_close();

	device->wait_idle();
}