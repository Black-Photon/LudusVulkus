#pragma once

#include "resource.h"

#include <memory>

#include "Application.h"
#include "Window.h"
#include "Instance.h"
#include "Settings.h"

class LudusVulkus {
public:
    LudusVulkus(std::unique_ptr<Application> app);
    void run();
    void run_loop();

private:
    std::unique_ptr<Application> app;
    std::unique_ptr<Window> window;
    std::shared_ptr<Instance> instance;
    std::shared_ptr<Settings> settings;

    void setup_settings();
    std::vector<std::string> prepare_extensions();
    std::vector<std::string> select_validation_layers();
};