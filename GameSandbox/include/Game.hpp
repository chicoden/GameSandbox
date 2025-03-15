#pragma once

#include <GLFW/glfw3.h>

namespace sandbox {
	class Game {
		public:
			Game();
			bool init();
			void run();
			~Game();

		private:
			GLFWmonitor* monitor;
			GLFWwindow* window;
			bool isFullscreen;
			int windowReturnPosX, windowReturnPosY;
			int windowReturnSizeX, windowReturnSizeY;

			static void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void handleMousePosition(GLFWwindow* window, double x, double y);

			bool createWindow();
			void setupEventHandlers();
	};
}