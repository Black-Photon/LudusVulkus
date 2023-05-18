#include "Application.h"

#include "Logger.h"
#include "SubpassDependency.h"

Application::Application(Instance& instance, Device& device, Window& window, Surface& surface, Settings& settings) {
    this->instance = &instance;
    this->device = &device;
    this->window = &window;
    this->surface = &surface;
    this->settings = settings;
}

Application::~Application() {
    Logger::log("Deleting base application");
}

void Application::prepare() {
    swap_chain = std::make_unique<SwapChain>(*device, *window, *surface, settings);

    command_pool = std::make_unique<CommandPool>(*device);
}

void Application::update() {

}

void Application::on_close() {

}

void Application::recreate_swapchain() {
    // If minimised, wait until it's reopened before continuing
    int width = 0, height = 0;
    glfwGetFramebufferSize(window->get(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window->get(), &width, &height);
        glfwWaitEvents();
    }

    device->wait_idle();

    swap_chain.reset();
    swap_chain = std::make_unique<SwapChain>(*device, *window, *surface, settings);
}