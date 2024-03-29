#include "Vulkus3D.h"

#include "Logger.h"

Vulkus3D::Vulkus3D(Instance& instance, Device& device, Window& window, Surface& surface, Settings& settings) :
	Application(instance, device, window, surface, settings)
{
}

Vulkus3D::~Vulkus3D() {
	Logger::log("Deleting Vulkus3D");
}

void Vulkus3D::prepare() {
	Application::prepare();

	Queue& transfer_queue = *device->queues.at(TRANSFER);

	render_pass = std::make_unique<GeometryRenderPass>(*device, *swap_chain);
	render_pass->create_buffers(*command_pool, transfer_queue);
	render_pass->prepare_framebuffers();
	render_pass->setup_descriptor_sets(FRAMES_IN_FLIGHT);
	render_pass->prepare_pipeline();
	render_pass->prepare_descriptor_sets(FRAMES_IN_FLIGHT);

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
		frames[i] = std::make_unique<Frame>(
			command_pool->create_command_buffer(),
			std::make_unique<Semaphore>(*device),
			std::make_unique<Semaphore>(*device),
			std::make_unique<Fence>(*device)
			);
	}
}

void Vulkus3D::update() {
	Application::update();

	Frame& frame = *frames.at(current_frame);

	frame.image_in_flight->wait();

	CommandBuffer& command_buffer = frame.command_buffer;
	Queue& graphics_queue = *device->queues.at(GRAPHICS);
	Queue& present_queue = *device->queues.at(PRESENT);
	ImageIndex image_index = swap_chain->get_next_image(*frame.image_available);

	frame.image_in_flight->reset();

	render_pass->update_descriptor_sets(swap_chain->get_extent().width, swap_chain->get_extent().height, current_frame);

	auto wait_semaphores = std::vector<std::pair<Semaphore*, VkPipelineStageFlags>>();
	wait_semaphores.push_back(std::pair(frame.image_available.get(), PipelineStage::ColourAttachmentOutput));

	auto signal_semaphores = std::vector<Semaphore*>();
	signal_semaphores.push_back(frame.render_finished.get());

	command_buffer.reset();
	command_buffer.start_recording();
	render_pass->record_commands(command_buffer, image_index, current_frame);
	command_buffer.stop_recording();

	graphics_queue.submit(command_buffer, wait_semaphores, signal_semaphores, frame.image_in_flight.get());

	present_queue.present(*swap_chain, image_index, signal_semaphores);

	current_frame = (current_frame + 1) % FRAMES_IN_FLIGHT;
}

void Vulkus3D::on_close() {
	Application::on_close();

	device->wait_idle();
}

void Vulkus3D::recreate_swapchain() {
	Application::recreate_swapchain();

	Logger::log("Refreshing framebuffers", Logger::VERBOSE);
	render_pass->update_swapchain(*swap_chain);
	render_pass->prepare_framebuffers();
}