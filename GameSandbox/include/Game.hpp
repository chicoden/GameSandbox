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

			static bool checkLayerSupport(const char* layer, const std::vector<VkLayerProperties>& availableLayers);
			static bool checkExtensionSupport(const char* extension, const std::vector<VkExtensionProperties>& availableExtensions);
			static bool requireRequiredLayers(std::vector<const char*>& enabledLayers, const std::vector<VkLayerProperties>& availableLayers);
			static bool requireRequiredExtensions(std::vector<const char*>& enabledExtensions, const std::vector<VkExtensionProperties>& availableExtensions);
			static void initDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
			static void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void handleMousePosition(GLFWwindow* window, double x, double y);

			bool initWindow();
			void setupEventHandlers();
			void toggleFullscreen();
			bool setupDebugMessenger();
			bool initVulkan();
			bool createVkInstance(const std::vector<const char*>& enabledLayers, const std::vector<const char*>& enabledExtensions);
	};
}