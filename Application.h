#pragma once

#include <string>

#include "Instance.h"

class Application {
public:
	std::string name;
	Version version;

	virtual void prepare();
	virtual void update();
};

