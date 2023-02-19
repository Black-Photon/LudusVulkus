#include "LudusVulkus.h"

#include <GLFW/glfw3.h>

#include "Logger.h"

LudusVulkus::LudusVulkus(std::unique_ptr<Application> app) :
    app{ std::move(app) }
{
    // Initialise Window
    window = std::make_unique<Window>(this->app->name, 800, 600, false);
}

void LudusVulkus::run() {
    Logger::log("Starting application");

    app->prepare(*window);
    
    run_loop();
}

void LudusVulkus::run_loop() {
    while (!window->should_close()) {
        glfwPollEvents();

        app->update();
    }
}