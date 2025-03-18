#include <cstdint>
#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include "Common.hpp"
#include "Game.hpp"

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger) {
	auto _vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (_vkCreateDebugUtilsMessengerEXT != nullptr) return _vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
	else return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator) {
	auto _vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (_vkDestroyDebugUtilsMessengerEXT != nullptr) _vkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

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
		instance(NULL),
		debugMessenger(VK_NULL_HANDLE)
	{
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt");
		auto logger = std::make_shared<spdlog::logger>(spdlog::logger("SandboxLogger", {consoleSink, fileSink}));
		spdlog::set_default_logger(logger);
	}

	bool Game::init() {
		if (!glfwInit()) {
			spdlog::critical("failed to initialize glfw");
			return false;
		} else {
			spdlog::info("initialized glfw");
		}

		if (!initWindow()) {
			spdlog::critical("failed to initialize window");
			return false;
		} else {
			spdlog::info("initialized window");
		}

		if (!initVulkan()) {
			spdlog::critical("failed to initialize vulkan");
			return false;
		} else {
			spdlog::info("initialized vulkan");
		}

		setupEventHandlers();
		spdlog::info("setup event handlers");

		return true;
	}

	void Game::run() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	Game::~Game() {
		if (ENABLE_VALIDATION_LAYERS) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
			spdlog::info("destroyed debug messenger");
		}

		vkDestroyInstance(instance, nullptr);
		spdlog::info("destroyed vulkan instance");

		glfwMakeContextCurrent(nullptr);
		glfwTerminate();
		spdlog::info("destroyed glfw resources");
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL Game::debugMessengerCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			spdlog::critical("validation layer: {}", pCallbackData->pMessage);
		} else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			spdlog::warn("validation layer: {}", pCallbackData->pMessage);
		} else { // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			//spdlog::info("validation layer: {}", pCallbackData->pMessage);
		}

		return VK_FALSE;
	}

	bool Game::checkLayerSupport(const char* layer, const std::vector<VkLayerProperties>& availableLayers) {
		for (const auto& layerProps : availableLayers) {
			if (strcmp(layer, layerProps.layerName) == 0) {
				return true;
			}
		}

		return false;
	}

	bool Game::checkExtensionSupport(const char* extension, const std::vector<VkExtensionProperties>& availableExtensions) {
		for (const auto& extProps : availableExtensions) {
			if (strcmp(extension, extProps.extensionName) == 0) {
				return true;
			}
		}

		return false;
	}

	bool Game::requireRequiredLayers(std::vector<const char*>& enabledLayers, const std::vector<VkLayerProperties>& availableLayers) {
		size_t firstRequiredLayerIndex = enabledLayers.size();
		if (ENABLE_VALIDATION_LAYERS) enabledLayers.push_back("VK_LAYER_KHRONOS_validation");

		bool haveRequiredLayers = true;
		for (size_t i = firstRequiredLayerIndex; i < enabledLayers.size(); i++) {
			const char* requiredLayer = enabledLayers[i];
			if (checkLayerSupport(requiredLayer, availableLayers)) {
				spdlog::info("required layer {}: supported", requiredLayer);
			} else {
				haveRequiredLayers = false;
				spdlog::critical("required layer {}: unsupported", requiredLayer);
			}
		}

		return haveRequiredLayers;
	}

	bool Game::requireRequiredExtensions(std::vector<const char*>& enabledExtensions, const std::vector<VkExtensionProperties>& availableExtensions) {
		uint32_t numRequiredGlfwExtensions;
		const char** requiredGlfwExtensions = glfwGetRequiredInstanceExtensions(&numRequiredGlfwExtensions);

		size_t firstRequiredExtensionIndex = enabledExtensions.size();
		enabledExtensions.insert(enabledExtensions.end(), requiredGlfwExtensions, requiredGlfwExtensions + numRequiredGlfwExtensions);
		if (ENABLE_VALIDATION_LAYERS) enabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		bool haveRequiredExtensions = true;
		for (size_t i = firstRequiredExtensionIndex; i < enabledExtensions.size(); i++) {
			const char* requiredExtension = enabledExtensions[i];
			if (checkExtensionSupport(requiredExtension, availableExtensions)) {
				spdlog::info("required extension {}: supported", requiredExtension);
			} else {
				haveRequiredExtensions = false;
				spdlog::critical("required extension {}: unsupported", requiredExtension);
			}
		}

		return haveRequiredExtensions;
	}

	void Game::initDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,

			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,

			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			//	| VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,

			.pfnUserCallback = debugMessengerCallback,
			.pUserData = nullptr,
		};
	}

	void Game::handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

		if (action == GLFW_PRESS && key == KEY_EXIT_FULLSCREEN) {
			game->toggleFullscreen();
		}
	}

	void Game::handleMousePosition(GLFWwindow* window, double x, double y) {}

	bool Game::initWindow() {
		monitor = glfwGetPrimaryMonitor();
		if (monitor == nullptr) {
			spdlog::critical("no monitors found");
			return false;
		}

		int numVideoModes;
		const GLFWvidmode* availableVideoModes = glfwGetVideoModes(monitor, &numVideoModes);
		spdlog::info("available video modes for primary monitor: {}", join(availableVideoModes, availableVideoModes + numVideoModes, ", ", [](const auto& videoMode) {
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
			spdlog::critical("failed to create window");
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

	bool Game::setupDebugMessenger() {
		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo;
		initDebugMessengerCreateInfo(messengerCreateInfo);
		return CreateDebugUtilsMessengerEXT(instance, &messengerCreateInfo, nullptr, &debugMessenger) == VK_SUCCESS;
	}

	bool Game::initVulkan() {
		uint32_t numAvailableLayers;
		vkEnumerateInstanceLayerProperties(&numAvailableLayers, nullptr);
		std::vector<VkLayerProperties> availableLayers(numAvailableLayers);
		vkEnumerateInstanceLayerProperties(&numAvailableLayers, availableLayers.data());
		spdlog::info("available layers: {}", join(availableLayers.begin(), availableLayers.end(), ", ", [](const auto& layer) { return std::string(layer.layerName); }));

		std::vector<const char*> enabledLayers;
		if (!requireRequiredLayers(enabledLayers, availableLayers)) {
			spdlog::critical("some required layers not supported");
			return false;
		}

		uint32_t numAvailableExtensions;
		vkEnumerateInstanceExtensionProperties(nullptr, &numAvailableExtensions, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(numAvailableExtensions);
		vkEnumerateInstanceExtensionProperties(nullptr, &numAvailableExtensions, availableExtensions.data());
		spdlog::info("available extensions: {}", join(availableExtensions.begin(), availableExtensions.end(), ", ", [](const auto& ext) { return std::string(ext.extensionName); }));

		std::vector<const char*> enabledExtensions;
		if (!requireRequiredExtensions(enabledExtensions, availableExtensions)) {
			spdlog::critical("some required extensions not supported");
			return false;
		}

		if (!createVkInstance(enabledLayers, enabledExtensions)) {
			spdlog::critical("failed to create vulkan instance");
			return false;
		} else {
			spdlog::info("created vulkan instance");
		}

		if (ENABLE_VALIDATION_LAYERS) {
			if (!setupDebugMessenger()) {
				spdlog::critical("failed to setup debug messenger");
				return false;
			} else {
				spdlog::info("setup debug messenger");
			}
		}

		return true;
	}

	bool Game::createVkInstance(const std::vector<const char*>& enabledLayers, const std::vector<const char*>& enabledExtensions) {
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
		instCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
		instCreateInfo.ppEnabledLayerNames = enabledLayers.data();
		instCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
		instCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();

		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo;
		if (ENABLE_VALIDATION_LAYERS) {
			initDebugMessengerCreateInfo(messengerCreateInfo);
			instCreateInfo.pNext = &messengerCreateInfo;
		}

		return vkCreateInstance(&instCreateInfo, nullptr, &instance) == VK_SUCCESS;
	}
}