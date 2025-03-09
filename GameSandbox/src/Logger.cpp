#include <iostream>
#include "Logger.hpp"

using namespace sandbox;

Logger::Logger(const fs::path& path) : logFile(path) {
	if (!logFile.is_open()) log(LogLevel::ERROR, "failed to open log file, will only log to stdout");
	log(LogLevel::INFO, "logging on");
}

void Logger::log(LogLevel level, const std::string& message) {
	std::cout << LEVEL_NAMES[level] << ": " << message << std::endl;
	if (logFile.is_open()) logFile << LEVEL_NAMES[level] << ": " << message << std::endl;
}

Logger::~Logger() {
	log(LogLevel::INFO, "logging off");
}