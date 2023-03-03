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

    SubpassDependency dependancy;
    dependancy.set_src_subpass(VK_SUBPASS_EXTERNAL);
    dependancy.set_dest_subpass(0);
    dependancy.add_src_stage(ColourAttachmentOutput); // Only when previous has finished writing...
    dependancy.add_dest_stage(ColourAttachmentOutput); // will we write the current subpass
    dependancy.add_dest_access(ColourAttachmentWrite); // Only write color

    render_pass = std::make_unique<RenderPass>(*device, swap_chain->image_format, std::vector{ dependancy });

    for (auto& image_view : swap_chain->image_views) {
        auto framebuffer = std::make_unique<Framebuffer>(*device, *render_pass, *image_view, *swap_chain);
        framebuffers.push_back(std::move(framebuffer));
    }

    Shader vertex_shader(*device, "Vertex.spv");
    Shader fragment_shader(*device, "Fragment.spv");
    pipeline = std::make_unique<Pipeline>(vertex_shader, fragment_shader, *device, *render_pass);

    command_pool = std::make_unique<CommandPool>(*device);
}

void Application::update() {

}

void Application::on_close() {

}

void Application::recreate_swapchain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window->get(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window->get(), &width, &height);
        glfwWaitEvents();
    }

    device->wait_idle();

    swap_chain = std::make_unique<SwapChain>(*device, *window, *surface, settings);

    framebuffers = std::vector<std::unique_ptr<Framebuffer>>();

    for (auto& image_view : swap_chain->image_views) {
        auto framebuffer = std::make_unique<Framebuffer>(*device, *render_pass, *image_view, *swap_chain);
        framebuffers.push_back(std::move(framebuffer));
    }
}