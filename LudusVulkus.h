#pragma once

#include "resource.h"

#include <memory>

#include "Application.h"
#include "Window.h"

class LudusVulkus {
public:
    LudusVulkus(std::unique_ptr<Application> app);
    void run();
    void run_loop();

private:
    std::unique_ptr<Application> app;
    std::unique_ptr<Window> window;
};