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

	Queue& transfer_queue = *device->queues.at(TRANSFER);

	render_pass = std::make_unique<TriangleRenderPass>(*device, *swap_chain);
	render_pass->prepare_framebuffers();
	render_pass->prepare_pipeline(*command_pool, transfer_queue);

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

	auto wait_semaphores = std::vector<std::pair<Semaphore*, VkPipelineStageFlags>>();
	wait_semaphores.push_back(std::pair(frame.image_available.get(), PipelineStage::ColourAttachmentOutput));

	auto signal_semaphores = std::vector<Semaphore*>();
	signal_semaphores.push_back(frame.render_finished.get());

	command_buffer.reset();
	command_buffer.start_recording();
	render_pass->record_commands(command_buffer, image_index);
	command_buffer.stop_recording();

	graphics_queue.submit(command_buffer, wait_semaphores, signal_semaphores, frame.image_in_flight.get());

	present_queue.present(*swap_chain, image_index, signal_semaphores);

	while (true);

	current_frame = (current_frame + 1) % FRAMES_IN_FLIGHT;
}

void TriangleEngine::on_close() {
	Application::on_close();

	device->wait_idle();
}

void TriangleEngine::recreate_swapchain() {
	Application::recreate_swapchain();

	Logger::log("Refreshing framebuffers", Logger::VERBOSE);
	render_pass->update_swapchain(*swap_chain);
	render_pass->prepare_framebuffers();
}