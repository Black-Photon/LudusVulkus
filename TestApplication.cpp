#include "TestApplication.h"

TestApplication::TestApplication() {
	name = "Test Application";
	version = { 1, 0, 0 };
}

void TestApplication::prepare() {
	Application::prepare();
}

void TestApplication::update() {
	Application::update();
}