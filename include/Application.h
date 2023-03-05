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
	static std::string name;
	static Version version;

    Application(Instance& instance, Device& device, Window& window, Surface& surface, Settings& settings);
    virtual ~Application();

	virtual void prepare();
	virtual void update();
    virtual void recreate_swapchain();
    virtual void on_close();

protected:
    Instance* instance;
    Device* device;
    Window* window;
    Surface* surface;
    Settings settings;

    std::unique_ptr<SwapChain> swap_chain;
    std::unique_ptr<CommandPool> command_pool;
};

