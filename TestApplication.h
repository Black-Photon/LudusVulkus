#pragma once

#include "Application.h"

class TestApplication : public Application {
public:
	TestApplication();

	void prepare() override;
	void update() override;
};

