#pragma once

#include <string>
#include <fstream>
#include "Common.hpp"

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

#ifdef ENABLE_GLOBAL_LOGGING
	extern Logger logger;
	#define LOG(level, message) logger.log(level, message)
#else
	#define LOG(level, message)
#endif
}