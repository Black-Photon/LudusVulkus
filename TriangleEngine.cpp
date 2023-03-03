#include "TriangleEngine.h"

#include "Logger.h"

TriangleEngine::TriangleEngine(Instance& instance, Device& device, Window& window, Surface& surface, Settings& settings) :
	Application(instance, device, window, surface, settings)
{
}

TriangleEngine::~TriangleEngine() {
	Logger::log("Deleting triangle engine");
}

void TriangleEngine::prepare() {
	Application::prepare();

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
		frames[i] = std::make_unique<Frame>(
			command_pool->create_command_buffer(),
			std::make_unique<Semaphore>(*device),
			std::make_unique<Semaphore>(*device),
			std::make_unique<Fence>(*device)
		);
	}
}

void TriangleEngine::update() {
	Application::update();

	Frame& frame = *frames.at(current_frame);

	frame.image_in_flight->wait();

	CommandBuffer& command_buffer = frame.command_buffer;
	Queue& graphics_queue = *device->queues.at(GRAPHICS);
	Queue& present_queue = *device->queues.at(PRESENT);
	ImageIndex image_index = swap_chain->get_next_image(*frame.image_available);

	frame.image_in_flight->reset();

	auto wait_semaphores = std::vector<std::pair<Semaphore*, PipelineStage>>();
	wait_semaphores.push_back(std::pair(frame.image_available.get(), ColourAttachmentOutput));

	auto signal_semaphores = std::vector<Semaphore*>();
	signal_semaphores.push_back(frame.render_finished.get());

	command_buffer.reset();
	command_buffer.start_recording();

	command_buffer.cmd_begin_render_pass(*render_pass, *framebuffers.at(image_index));
	command_buffer.cmd_bind_pipeline(*pipeline);
	command_buffer.cmd_set_scissor();
	command_buffer.cmd_set_viewport();
	command_buffer.cmd_draw();
	command_buffer.cmd_end_render_pass();

	command_buffer.stop_recording();

	graphics_queue.submit(command_buffer, wait_semaphores, signal_semaphores, frame.image_in_flight.get());

	present_queue.present(*swap_chain, image_index, signal_semaphores);

	current_frame = (current_frame + 1) % FRAMES_IN_FLIGHT;
}

void TriangleEngine::on_close() {
	Application::on_close();

	device->wait_idle();
}