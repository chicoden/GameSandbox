#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <filesystem>
#include <initializer_list>
#include <utility>
#include "Logger.hpp"

namespace fs = std::filesystem;

namespace sandbox {
	class Game {
		public:
			Game();
			bool run();
			~Game();

		private:
			static void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void handleMousePosition(GLFWwindow* window, double x, double y);

			bool createWindow();
			void destroyWindow();
			void setupEventHandlers();
			unsigned int compileProgram(std::initializer_list<std::pair<fs::path, GLenum>> sources);

			inline static const std::string GAME_TITLE = "Game Sandbox";
			static const int DEFAULT_WINDOW_SIZE_X = 800;
			static const int DEFAULT_WINDOW_SIZE_Y = 600;

			Logger logger;
			GLFWwindow* window;
	};
}