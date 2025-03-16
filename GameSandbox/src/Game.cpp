#include <cstdint>
#include <string>
#include <cstring>
#include <vector>
#include "Common.hpp"
#include "Game.hpp"

namespace sandbox {
	Game::Game() :
		monitor(nullptr),
		videoMode(nullptr),
		window(nullptr),
		isFullscreen(false),
		windowReturnPosX(0),
		windowReturnPosY(0),
		windowReturnSizeX(DEFAULT_WINDOW_SIZE_X),
		windowReturnSizeY(DEFAULT_WINDOW_SIZE_Y),
		vkInstance(nullptr)
	{
		spdlog::info("game instantiated");
	}

	bool Game::init() {
		if (!glfwInit()) {
			spdlog::critical("failed to initialize glfw");
			return false;
		} else {
			spdlog::info("initialized glfw");
		}

		if (!createWindow()) {
			spdlog::critical("failed to create window");
			return false;
		}

		if (!initVulkan()) {
			spdlog::critical("failed to initialize vulkan");
			return false;
		} else {
			spdlog::info("done setting up vulkan");
		}

		setupEventHandlers();
		spdlog::info("event handlers registered");

		return true;
	}

	void Game::run() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	Game::~Game() {
		vkDestroyInstance(vkInstance, nullptr);
		spdlog::info("destroyed vulkan instance");

		glfwMakeContextCurrent(nullptr);
		glfwTerminate();
		spdlog::info("glfw terminated, all remaining glfw resources destroyed");

		spdlog::info("game destroyed");
	}

	void Game::handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

		if (action == GLFW_PRESS && key == KEY_EXIT_FULLSCREEN) {
			game->toggleFullscreen();
		}
	}

	void Game::handleMousePosition(GLFWwindow* window, double x, double y) {}

	bool Game::createWindow() {
		monitor = glfwGetPrimaryMonitor();
		if (monitor == nullptr) {
			spdlog::critical("no monitors found");
			return false;
		}

		int numVideoModes;
		const GLFWvidmode* availableVideoModes = glfwGetVideoModes(monitor, &numVideoModes);
		spdlog::info("available video modes for primary monitor: {}", stringJoin(availableVideoModes, availableVideoModes + numVideoModes, ", ", [](const auto& videoMode) {
			return fmt::format("{}x{}@{}Hz R{}G{}B{}", videoMode.width, videoMode.height, videoMode.refreshRate, videoMode.redBits, videoMode.greenBits, videoMode.blueBits);
		}));

		videoMode = glfwGetVideoMode(monitor);
		if (videoMode == nullptr) {
			spdlog::critical("failed to get current video mode");
			return false;
		} else {
			spdlog::info("selecting current video mode {}x{}@{}Hz R{}G{}B{}", videoMode->width, videoMode->height, videoMode->refreshRate, videoMode->redBits, videoMode->greenBits, videoMode->blueBits);
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(DEFAULT_WINDOW_SIZE_X, DEFAULT_WINDOW_SIZE_Y, GAME_TITLE.c_str(), nullptr, nullptr);
		if (window == nullptr) {
			return false;
		} else {
			spdlog::info("created window");
		}

		//toggleFullscreen();

		return true;
	}

	void Game::setupEventHandlers() {
		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, handleKey);
		glfwSetCursorPosCallback(window, handleMousePosition);
	}

	void Game::toggleFullscreen() {
		if (isFullscreen) {
			spdlog::info("switching to windowed mode");
			glfwSetWindowMonitor(window, nullptr, windowReturnPosX, windowReturnPosY, windowReturnSizeX, windowReturnSizeY, GLFW_DONT_CARE);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			isFullscreen = false;
		} else {
			spdlog::info("switching to fullscreen mode");
			glfwGetWindowPos(window, &windowReturnPosX, &windowReturnPosY);
			glfwGetWindowSize(window, &windowReturnSizeX, &windowReturnSizeY);
			glfwSetWindowMonitor(window, monitor, 0, 0, videoMode->width, videoMode->height, GLFW_DONT_CARE);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			isFullscreen = true;
		}
	}

	bool Game::checkExtensionSupport(const char* extension, std::vector<VkExtensionProperties>& availableExtensions) {
		for (const auto& extProps : availableExtensions) {
			if (strcmp(extension, extProps.extensionName) == 0) {
				return true;
			}
		}

		return false;
	}

	bool Game::checkLayerSupport(const char* layer, std::vector<VkLayerProperties>& availableLayers) {
		for (const auto& layerProps : availableLayers) {
			if (strcmp(layer, layerProps.layerName) == 0) {
				return true;
			}
		}

		return false;
	}

	bool Game::initVulkan() {
		// Get list of available layers
		uint32_t numAvailableLayers;
		vkEnumerateInstanceLayerProperties(&numAvailableLayers, nullptr);
		std::vector<VkLayerProperties> availableLayers(numAvailableLayers);
		vkEnumerateInstanceLayerProperties(&numAvailableLayers, availableLayers.data());

		// TODO: check for validation layer support

		// Get list of available extensions
		uint32_t numAvailableExtensions;
		vkEnumerateInstanceExtensionProperties(nullptr, &numAvailableExtensions, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(numAvailableExtensions);
		vkEnumerateInstanceExtensionProperties(nullptr, &numAvailableExtensions, availableExtensions.data());
		spdlog::info("available extensions: {}", stringJoin(availableExtensions.begin(), availableExtensions.end(), ", ", [](const auto& ext) { return std::string(ext.extensionName); }));

		// Get GLFW required extensions
		uint32_t numGlfwRequiredExtensions;
		const char** glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&numGlfwRequiredExtensions);

		// Initialize array of enabled extensions with the extensions required by GLFW
		// May add additional required extensions in the future, and optional ones will be added if supported
		std::vector<const char*> enabledExtensions(glfwRequiredExtensions, glfwRequiredExtensions + numGlfwRequiredExtensions);

		// Check support for each required extension
		// Ultimately, they all need to be supported but it may be useful to know exactly which ones are and which aren't
	    bool haveRequiredExtensions = true;
		for (const char* extName : enabledExtensions) {
			if (checkExtensionSupport(extName, availableExtensions)) {
				spdlog::info("required extension {}: supported", extName);
			} else {
				haveRequiredExtensions = false;
				spdlog::critical("required extension {}: unsupported", extName);
			}
		}

		if (!haveRequiredExtensions) {
			spdlog::critical("some required extensions not supported");
			return false;
		}

		if (!createVkInstance(enabledExtensions)) {
			spdlog::critical("failed to create vulkan instance");
			return false;
		} else {
			spdlog::info("created vulkan instance");
		}

		return true;
	}

	bool Game::createVkInstance(std::vector<const char*>& enabledExtensions) {
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = GAME_TITLE.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = GAME_TITLE.c_str();
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo instCreateInfo{};
		instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instCreateInfo.pApplicationInfo = &appInfo;
		instCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
		instCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
		instCreateInfo.enabledLayerCount = 0;

		return vkCreateInstance(&instCreateInfo, nullptr, &vkInstance) == VK_SUCCESS;
	}
}