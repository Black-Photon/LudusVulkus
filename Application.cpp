#include "Application.h"

#include "Logger.h"
#include "SubpassDependency.h"

Application::~Application() {
    Logger::log("Deleting base application");
}

void Application::prepare(Window &window) {
    // Setup any constants
    setup_settings();

    std::set<std::string> validation_layers = select_validation_layers();
    std::set<std::string> device_extensions = prepare_device_extensions();

    // Initialise Instance
    instance = std::make_unique<Instance>(
        name, version,                          // App details
        "Ludus Vulkus", Version{ 1, 0, 0 },     // Engine details
        VK_API_VERSION_1_0,                     // Vulkan version
        settings,                               // Reference to settings
        prepare_extensions(),                   // Extensions to load
        validation_layers                       // Validation layers to load
        );

    surface = std::make_unique<Surface>(*instance, window);

    physical_devices = PhysicalDevice::get_device_list(*instance, *surface, device_extensions);
    for (auto& physical_device : physical_devices) {
        auto device = std::make_unique<Device>(physical_device, physical_device.selected_family.at(GRAPHICS), settings, device_extensions, validation_layers);
        devices.emplace_back(std::move(device));
    }

    main_device = devices.at(0).get();

    // Just use the first device for now
    swap_chain = std::make_unique<SwapChain>(*main_device, window, *surface, settings);

    SubpassDependency dependancy;
    dependancy.set_src_subpass(VK_SUBPASS_EXTERNAL);
    dependancy.set_dest_subpass(0);
    dependancy.add_src_stage(ColourAttachmentOutput); // Only when previous has finished writing...
    dependancy.add_dest_stage(ColourAttachmentOutput); // will we write the current subpass
    dependancy.add_dest_access(ColourAttachmentWrite); // Only write color

    render_pass = std::make_unique<RenderPass>(*main_device, *swap_chain, std::vector{ dependancy });

    Shader vertex_shader(*main_device, "Vertex.spv");
    Shader fragment_shader(*main_device, "Fragment.spv");
    pipeline = std::make_unique<Pipeline>(vertex_shader, fragment_shader, *main_device, *render_pass);

    for (auto& image_view : swap_chain->image_views) {
        auto framebuffer = std::make_unique<Framebuffer>(*main_device, *render_pass, *image_view, *swap_chain);
        framebuffers.push_back(std::move(framebuffer));
    }

    command_pool = std::make_unique<CommandPool>(*main_device);
    CommandBuffer& command_buffer = command_pool->create_command_buffer();
}

void Application::update() {

}

void Application::setup_settings() {
#ifdef NDEBUG
    bool debug = false;
#else
    bool debug = true;
#endif
    settings.use_validation_layers = debug;
    settings.close_on_non_fatal = false;

    settings.layer_verbose_enable = false;
    settings.layer_info_enable = false;
    settings.layer_warn_enable = true;
    settings.layer_error_enable = true;

    settings.is_mobile = is_mobile();
}

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

bool Application::is_mobile() {
    // From https://stackoverflow.com/a/5920028
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    // For now assume that all windows devices aren't on mobile (sorry Microsoft)
    return false;
#elif __APPLE__
#if TARGET_IPHONE_SIMULATOR
    // iOS, tvOS, or watchOS Simulator
    return true;
#elif TARGET_OS_MACCATALYST
    // Mac's Catalyst (ports iOS API into Mac, like UIKit).
    return false;
#elif TARGET_OS_IPHONE
    // iOS, tvOS, or watchOS device
    return true;
#elif TARGET_OS_MAC
    // Other kinds of Apple platforms
    return false;
#else
    Logger::log("Unknown Apple Device: Assuming non-mobile", Logger::NONFATAL);
    return false;
#endif
#elif __ANDROID__
    return true;
#elif __linux__
    return false;
#elif __unix__
    return false;
#elif defined(_POSIX_VERSION)
    return false;
#else
    Logger::log("Unknown compiler target: Assuming non-mobile", Logger::NONFATAL);
    return false;
#endif
}

std::set<std::string> Application::prepare_extensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    // Find extensions needed by glfw
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::set<std::string> extensions = std::set<std::string>(glfwExtensions, glfwExtensions + glfwExtensionCount);

    // Add extensions for validation layer if needed
    if (settings.use_validation_layers) extensions.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

std::set<std::string> Application::prepare_device_extensions() {
    std::set<std::string> layers = std::set<std::string>();
    layers.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    return layers;
}

std::set<std::string> Application::select_validation_layers() {
    std::set<std::string> layers = std::set<std::string>();
    layers.insert("VK_LAYER_KHRONOS_validation");
    return layers;
}