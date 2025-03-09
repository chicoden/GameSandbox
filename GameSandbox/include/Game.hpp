#pragma once

#include <GLFW/glfw3.h>
#include "Logger.hpp"

namespace sandbox {
	class Game {
		public:
			Game();
			bool run();
			~Game();

		private:
			static void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void handleMousePosition(GLFWwindow* window, double x, double y);

			void setupEventHandlers();
			bool createWindow();
			void destroyWindow();

			const char GAME_TITLE[13] = "Game Sandbox";
			static const int DEFAULT_WINDOW_SIZE_X = 800;
			static const int DEFAULT_WINDOW_SIZE_Y = 600;

			Logger logger;
			GLFWwindow* window;
	};
}