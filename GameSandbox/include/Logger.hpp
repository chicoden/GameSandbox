#pragma once

#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace gsbox {
	enum LogLevel {
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		CRITICAL
	};

	const std::string LEVEL_NAMES[] = {
		"DEBUG",
		"INFO",
		"WARNING",
		"ERROR",
		"CRITICAL"
	};

	class Logger {
		public:
			Logger(const fs::path& path);
			void log(LogLevel level, const std::string& message);
			~Logger();

		private:
			std::ofstream logFile;
	};
}