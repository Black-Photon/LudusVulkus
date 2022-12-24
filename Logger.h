#pragma once

#include <string>
#include <iostream>

#include "Settings.h"

namespace Logger {
	enum LogLevel {
		INFO,
		WARN,
		VERBOSE,
		FATAL,
		NONFATAL
	};

	static void log(std::string message, LogLevel level = INFO) {
		switch (level) {
		case INFO:
			std::cout << "[INFO]     " << message << std::endl;
			break;
		case WARN:
			std::cout << "[WARN]     " << message << std::endl;
			break;
		case VERBOSE:
			std::cout << "[VERBOSE]  " << message << std::endl;
			break;
		case FATAL:
			std::cerr << "[FATAL]    " << message << std::endl;
			break;
		case NONFATAL:
			std::cout << "[NONFATAL] " << message << std::endl;
			break;
		default:
			log("Unknown error type", NONFATAL);
		}
	}

	static void check_except_non_fatal(Settings settings) {
		if (settings.close_on_non_fatal) {
			throw std::runtime_error("As close_on_non_fatal is set, the preceeding non-fatal error is treated as fatal");
		}
	}
}