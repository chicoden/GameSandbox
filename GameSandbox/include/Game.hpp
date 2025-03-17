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

			VkInstance instance;
			VkDebugUtilsMessengerEXT debugMessenger;

			static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData
			);

			static void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void handleMousePosition(GLFWwindow* window, double x, double y);

			bool initWindow();
			void setupEventHandlers();
			void toggleFullscreen();

			bool checkLayerSupport(const char* layer, std::vector<VkLayerProperties>& availableLayers);
			bool checkExtensionSupport(const char* extension, std::vector<VkExtensionProperties>& availableExtensions);
			bool requireRequiredLayers(std::vector<const char*>& enabledLayers, std::vector<VkLayerProperties>& availableLayers);
			bool requireRequiredExtensions(std::vector<const char*>& enabledExtensions, std::vector<VkExtensionProperties>& availableExtensions);
			void initDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
			bool setupDebugMessenger();
			bool initVulkan();
			bool createVkInstance(std::vector<const char*>& enabledLayers, std::vector<const char*>& enabledExtensions);
	};
}