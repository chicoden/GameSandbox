#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Logger.hpp"
#include "Game.hpp"

namespace sandbox {
	Game::Game() : window(nullptr) {
		LOG(Logger::LogLevel::INFO, "game instantiated");
	}

	bool Game::init() {
		if (!createWindow()) {
			LOG(Logger::LogLevel::CRITICAL, "failed to set up window");
			return false;
		}

		if (!setupEventHandlers()) {
			LOG(Logger::LogLevel::CRITICAL, "failed to set up event handlers");
			return false;
		}

		return true;
	}

	void Game::run() {
		while (!glfwWindowShouldClose(window)) {
			glClearColor(1.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	Game::~Game() {
		destroyWindow();
		LOG(Logger::LogLevel::INFO, "game destroyed");
	}

	bool Game::createWindow() {
		if (!glfwInit()) {
			LOG(Logger::LogLevel::CRITICAL, "failed to initialize glfw");
			return false;
		}

		int windowSizeX = DEFAULT_WINDOW_SIZE_X;
		int windowSizeY = DEFAULT_WINDOW_SIZE_Y;

		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		if (primaryMonitor == nullptr) {
			LOG(Logger::LogLevel::CRITICAL, "no primary monitor");
			return false;
		}

		const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
		if (videoMode != nullptr) {
			windowSizeX = videoMode->width;
			windowSizeY = videoMode->height;

			LOG(
				Logger::LogLevel::INFO,
				std::string("current video mode ")
				+ std::to_string(videoMode->width) + "x" + std::to_string(videoMode->height) + "@" + std::to_string(videoMode->refreshRate) + "Hz "
				+ "R" + std::to_string(videoMode->redBits) + "G" + std::to_string(videoMode->greenBits) + "B" + std::to_string(videoMode->blueBits)
			);
		} else {
			LOG(
				Logger::LogLevel::INFO,
				std::string("failed to get current video mode, defaulting to ")
				+ std::to_string(windowSizeX) + "x" + std::to_string(windowSizeY)
			);
		}

		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(windowSizeX, windowSizeY, GAME_TITLE.c_str(), primaryMonitor, nullptr);
		if (window == nullptr) {
			LOG(Logger::LogLevel::CRITICAL, "failed to create window");
			return false;
		}

		glfwMakeContextCurrent(window);
		int ctxVersion = gladLoadGL(glfwGetProcAddress);
		if (ctxVersion == 0) {
			LOG(Logger::LogLevel::CRITICAL, "failed to create opengl context");
			return false;
		} else {
			LOG(
				Logger::LogLevel::INFO,
				std::string("created opengl ")
				+ std::to_string(GLAD_VERSION_MAJOR(ctxVersion)) + "." + std::to_string(GLAD_VERSION_MINOR(ctxVersion))
				+ " core context"
			);
		}

		return true;
	}

	void Game::handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}

	void Game::handleMousePosition(GLFWwindow* window, double x, double y) {
		LOG(Logger::LogLevel::INFO, std::string("mouse at ") + std::to_string(x) + ", " + std::to_string(y));
	}

	bool Game::setupEventHandlers() {
		glfwSetWindowUserPointer(window, this);

		/*if (glfwSetKeyCallback(window, handleKey) == nullptr) {
			LOG(Logger::LogLevel::CRITICAL, "failed to set key callback");
			return false;
		}

		if (glfwSetCursorPosCallback(window, handleMousePosition) == nullptr) {
			LOG(Logger::LogLevel::CRITICAL, "failed to set mouse position callback");
			return false;
		}*/

		// TODO: figure out why these always return NULL but seem to work
		glfwSetKeyCallback(window, handleKey);
		glfwSetCursorPosCallback(window, handleMousePosition);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		return true;
	}

	void Game::destroyWindow() {
		glfwMakeContextCurrent(nullptr);
		glfwTerminate();
		LOG(Logger::LogLevel::INFO, "terminated glfw, any remaining glfw resources destroyed");
	}
}