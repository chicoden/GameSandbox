#include <vulkan/vulkan.h>
#include <string>
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
		windowReturnSizeY(DEFAULT_WINDOW_SIZE_Y)
	{
		spdlog::info("game instantiated");
	}

	bool Game::init() {
		if (!glfwInit()) {
			spdlog::critical("failed to initialize glfw");
			return false;
		}

		if (!createWindow()) {
			spdlog::critical("failed to create window");
			return false;
		}

		setupEventHandlers();

		return true;
	}

	void Game::run() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	Game::~Game() {
		glfwMakeContextCurrent(nullptr);
		glfwTerminate();
		spdlog::info("game destroyed");
	}

	void Game::handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

		if (action == GLFW_PRESS && key == KEY_EXIT_FULLSCREEN) {
			game->toggleFullscreen();
		}
	}

	void Game::handleMousePosition(GLFWwindow* window, double x, double y) {
		spdlog::info("mouse at {}, {}", x, y);
	}

	bool Game::createWindow() {
		monitor = glfwGetPrimaryMonitor();
		if (monitor == nullptr) {
			spdlog::critical("no monitors found");
			return false;
		}

		int numVideoModes;
		const GLFWvidmode* availableVideoModes = glfwGetVideoModes(monitor, &numVideoModes);
		std::string videoModeInfo = "available video modes for primary monitor: ";
		for (int i = 0; i < numVideoModes; i++) {
			const GLFWvidmode* videoMode = &availableVideoModes[i];
			videoModeInfo += fmt::format("{}x{}@{}Hz R{}G{}B{}, ", videoMode->width, videoMode->height, videoMode->refreshRate, videoMode->redBits, videoMode->greenBits, videoMode->blueBits);
		}

		videoModeInfo.erase(videoModeInfo.end() - 2, videoModeInfo.end());
		spdlog::info(videoModeInfo);

		videoMode = glfwGetVideoMode(monitor);
		if (videoMode == nullptr) {
			spdlog::critical("failed to get current video mode");
			return false;
		} else {
			spdlog::info("selecting current video mode {}x{}@{}Hz R{}G{}B{}", videoMode->width, videoMode->height, videoMode->refreshRate, videoMode->redBits, videoMode->greenBits, videoMode->blueBits);
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(DEFAULT_WINDOW_SIZE_X, DEFAULT_WINDOW_SIZE_Y, GAME_TITLE.c_str(), nullptr, nullptr);
		if (window == nullptr) return false;
		toggleFullscreen();

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
}