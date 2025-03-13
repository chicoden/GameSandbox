#include <string>
#include <iostream>
#include "Common.hpp"
#include "Logger.hpp"

namespace sandbox {
	Logger::Logger(const std::string& name, const std::string& outputFilePath) : name(name), logFile(outputFilePath) {
		if (!logFile.is_open()) log(LogLevel::ERROR, std::string("failed to open ") + outputFilePath + ", logging to stdout only instead");
		log(LogLevel::INFO, "logging on");
	}

	void Logger::log(LogLevel level, const std::string& message) {
		std::cout << name << ":" << LEVEL_NAMES[level] << ": " << message << std::endl;
		if (logFile.is_open()) logFile << name << ":" << LEVEL_NAMES[level] << ": " << message << std::endl;
	}

	Logger::~Logger() {
		log(LogLevel::INFO, "logging off");
	}

#ifdef ENABLE_GLOBAL_LOGGING
	Logger logger(GLOBAL_LOGGER_NAME, GLOBAL_LOGGER_PATH);
#endif
}