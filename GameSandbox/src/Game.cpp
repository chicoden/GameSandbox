#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Logger.hpp"
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
		LOG(Logger::LogLevel::INFO, "game instantiated");
	}

	bool Game::init() {
		if (!glfwInit()) {
			LOG(Logger::LogLevel::CRITICAL, "failed to initialize glfw");
			return false;
		}

		if (!createWindow()) {
			LOG(Logger::LogLevel::CRITICAL, "failed to create window");
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
		LOG(Logger::LogLevel::INFO, "game destroyed");
	}

	void Game::handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

		if (action == GLFW_PRESS && key == KEY_EXIT_FULLSCREEN) {
			game->toggleFullscreen();
		}
	}

	void Game::handleMousePosition(GLFWwindow* window, double x, double y) {
		LOG(Logger::LogLevel::INFO, std::string("mouse at ") + std::to_string(x) + ", " + std::to_string(y));
	}

	bool Game::createWindow() {
		monitor = glfwGetPrimaryMonitor();
		if (monitor == nullptr) {
			LOG(Logger::LogLevel::CRITICAL, "no monitors found");
			return false;
		}

		int numVideoModes;
		const GLFWvidmode* availableVideoModes = glfwGetVideoModes(monitor, &numVideoModes);
		std::string videoModeInfo = "available video modes for primary monitor:\n";
		for (int i = 0; i < numVideoModes; i++) {
			const GLFWvidmode* videoMode = &availableVideoModes[i];
			videoModeInfo += std::string("    ")
				+ std::to_string(videoMode->width) + "x" + std::to_string(videoMode->height) + "@" + std::to_string(videoMode->refreshRate) + "Hz "
				+ "R" + std::to_string(videoMode->redBits) + "G" + std::to_string(videoMode->greenBits) + "B" + std::to_string(videoMode->blueBits) + "\n";
		}

		LOG(Logger::LogLevel::INFO, videoModeInfo);

		videoMode = glfwGetVideoMode(monitor);
		if (videoMode == nullptr) {
			LOG(Logger::LogLevel::CRITICAL, "failed to get current video mode");
			return false;
		} else {
			LOG(
				Logger::LogLevel::INFO,
				std::string("selecting current video mode ")
				+ std::to_string(videoMode->width) + "x" + std::to_string(videoMode->height) + "@" + std::to_string(videoMode->refreshRate) + "Hz "
				+ "R" + std::to_string(videoMode->redBits) + "G" + std::to_string(videoMode->greenBits) + "B" + std::to_string(videoMode->blueBits)
			);
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
			glfwSetWindowMonitor(window, nullptr, windowReturnPosX, windowReturnPosY, windowReturnSizeX, windowReturnSizeY, GLFW_DONT_CARE);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			isFullscreen = false;
		} else {
			glfwGetWindowPos(window, &windowReturnPosX, &windowReturnPosY);
			glfwGetWindowSize(window, &windowReturnSizeX, &windowReturnSizeY);
			glfwSetWindowMonitor(window, monitor, 0, 0, videoMode->width, videoMode->height, GLFW_DONT_CARE);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			isFullscreen = true;
		}
	}
}