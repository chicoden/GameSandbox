#pragma once

#include <string>
#include <fstream>

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

			Logger(const std::string& name, const std::string& outputFilePath);
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

			std::string name;
			std::ofstream logFile;
	};

	extern Logger logger;
}