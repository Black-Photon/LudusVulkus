#include "Logger.h"

#include <iostream>

void Logger::log(std::string message, LogLevel level) {
	switch (level) {
	case INFO:
		std::cout << "[INFO]    " << message << std::endl;
		break;
	case WARN:
		std::cout << "[WARN]    " << message << std::endl;
		break;
	case ERROR:
		std::cerr << "[ERROR]   " << message << std::endl;
		break;
	case VERBOSE:
		std::cout << "[VERBOSE] " << message << std::endl;
		break;
	default:
		log("Unknown error type", ERROR);
	}
}