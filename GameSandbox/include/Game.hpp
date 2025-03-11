#pragma once

#include <GLFW/glfw3.h>
#include <string>

namespace sandbox {
	class Game {
		public:
			Game();
			bool init();
			void run();
			~Game();

		private:
			inline static const std::string GAME_TITLE = "Game Sandbox";
			static const int DEFAULT_WINDOW_SIZE_X = 800;
			static const int DEFAULT_WINDOW_SIZE_Y = 600;

			GLFWwindow* window;

			static void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void handleMousePosition(GLFWwindow* window, double x, double y);

			bool createWindow();
			bool setupEventHandlers();
			void destroyWindow();
	};
}