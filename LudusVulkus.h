#pragma once

#include "resource.h"

#include <memory>

#include "Application.h"
#include "Window.h"
#include "Instance.h"
#include "Settings.h"
#include "PhysicalDevice.h"

class LudusVulkus {
public:
    LudusVulkus(std::unique_ptr<Application> app);
    void run();
    void run_loop();

private:
    std::unique_ptr<Application> app;
    std::shared_ptr<Window> window;
    std::shared_ptr<Instance> instance;
    std::shared_ptr<Settings> settings;
    std::vector<PhysicalDevice> physical_devices;

    void setup_settings();
    std::vector<std::string> prepare_extensions();
    std::vector<std::string> select_validation_layers();
};