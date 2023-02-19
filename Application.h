#pragma once

#include <string>
#include <memory>

#include "Instance.h"
#include "Settings.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Queue.h"
#include "SwapChain.h"
#include "Surface.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Framebuffer.h"
#include "CommandBuffer.h"
#include "CommandPool.h"

class Application {
public:
	std::string name;
	Version version;

    virtual ~Application();

	virtual void prepare(Window& window);
	virtual void update();

protected:
    std::unique_ptr<Instance> instance;
    std::unique_ptr<Surface> surface;
    std::vector<PhysicalDevice> physical_devices;
    std::vector<std::unique_ptr<Device>> devices;

    Device *main_device;
    std::unique_ptr<SwapChain> swap_chain;
    std::unique_ptr<RenderPass> render_pass;
    std::unique_ptr<Pipeline> pipeline;
    std::vector<std::unique_ptr<Framebuffer>> framebuffers;
    std::unique_ptr<CommandPool> command_pool;

    Settings settings;

    void setup_settings();
    bool is_mobile();
    std::set<std::string> prepare_extensions();
    std::set<std::string> prepare_device_extensions();
    std::set<std::string> select_validation_layers();
};

