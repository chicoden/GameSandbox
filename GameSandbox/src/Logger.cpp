#include <iostream>
#include <stdexcept>
#include "Logger.hpp"

gsbox::Logger::Logger(const fs::path& path) {
	logFile.open(path);
	if (!logFile.is_open()) {
		throw std::runtime_error("failed to open log file");
	}
}

void gsbox::Logger::log(LogLevel level, const std::string& message) {
	std::cout << gsbox::LEVEL_NAMES[level] << ": " << message << std::endl;
	logFile << gsbox::LEVEL_NAMES[level] << ": " << message << std::endl;
}

gsbox::Logger::~Logger() {
	logFile.close();
}