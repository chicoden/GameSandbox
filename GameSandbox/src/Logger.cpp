#include <iostream>
#include <stdexcept>
#include "Logger.hpp"

using namespace gsbox;

Logger::Logger(const fs::path& path) {
	logFile.open(path);
	if (!logFile.is_open()) {
		throw std::runtime_error("failed to open log file");
	}
}

void Logger::log(LogLevel level, const std::string& message) {
	std::cout << LEVEL_NAMES[level] << ": " << message << std::endl;
	logFile << LEVEL_NAMES[level] << ": " << message << std::endl;
}

Logger::~Logger() {
	log(LogLevel::INFO, "logging off");
}