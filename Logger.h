#pragma once

#include <string>

class Logger {
public:
	enum LogLevel {
		INFO,
		WARN,
		ERROR,
		VERBOSE
	};

	static void log(std::string message, LogLevel level = INFO);
};

