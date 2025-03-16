#pragma once

#include <vector>
#include "Common.hpp"

namespace sandbox {
	class Game {
		public:
			Game();
			bool init();
			void run();
			~Game();

		private:
			GLFWmonitor* monitor;
			const GLFWvidmode* videoMode;
			GLFWwindow* window;
			bool isFullscreen;
			int windowReturnPosX, windowReturnPosY;
			int windowReturnSizeX, windowReturnSizeY;

			VkInstance vkInstance;

			static void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void handleMousePosition(GLFWwindow* window, double x, double y);

			bool createWindow();
			void setupEventHandlers();
			void toggleFullscreen();

			bool checkExtensionSupport(const char* extension, std::vector<VkExtensionProperties>& availableExtensions);
			bool checkLayerSupport(const char* layer, std::vector<VkLayerProperties>& availableLayers);
			bool initVulkan();
			bool createVkInstance(std::vector<const char*>& enabledExtensions);
	};
}