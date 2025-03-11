#pragma once

#include <string>
#include <fstream>
#include <iostream>

#define LOG(level, message) sandbox::logger.log(level, message)

namespace sandbox {
	class Logger {
	public:
		enum LogLevel {
			DEBUG,
			INFO,
			WARNING,
			ERROR,
			CRITICAL
		};

		Logger(const std::string& name, const std::string& outputFilePath) : name(name), logFile(outputFilePath) {
			if (!logFile.is_open()) log(LogLevel::ERROR, std::string("failed to open ") + outputFilePath + ", logging to stdout only instead");
			log(LogLevel::INFO, "logging on");
		}

		void log(LogLevel level, const std::string& message) {
			std::cout << name << ":" << LEVEL_NAMES[level] << ": " << message << std::endl;
			if (logFile.is_open()) logFile << name << ":" << LEVEL_NAMES[level] << ": " << message << std::endl;
		}

		~Logger() {
			log(LogLevel::INFO, "logging off");
		}

	private:
		inline static const std::string LEVEL_NAMES[] = {
			"DEBUG",
			"INFO",
			"WARNING",
			"ERROR",
			"CRITICAL"
		};

		std::string name;
		std::ofstream logFile;
	};

	static Logger logger("SandboxLogger", "log.txt");
}