#pragma once

#include <filesystem>
#include <string>
#include <fstream>

namespace fs = std::filesystem;

namespace sandbox {
	enum LogLevel {
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		CRITICAL
	};

	class Logger {
		public:
			Logger(const fs::path& path);
			void log(LogLevel level, const std::string& message);
			~Logger();

		private:
			inline static const std::string LEVEL_NAMES[] = {
				"DEBUG",
				"INFO",
				"WARNING",
				"ERROR",
				"CRITICAL"
			};

			std::ofstream logFile;
	};
}