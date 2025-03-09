#pragma once

#include <GLFW/glfw3.h>
#include "Logger.hpp"

namespace gsbox {
	class Game {
		public:
			Game();
			void run();
			~Game();

		private:
			static void handleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void handleMouseMoveEvent(GLFWwindow* window, double x, double y);
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