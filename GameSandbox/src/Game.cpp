#include <glad/gl.h>
#include <string>
#include <stdexcept>
#include "Game.hpp"

using namespace gsbox;

Game::Game() : logger("log.txt"), window(nullptr) {
	logger.log(LogLevel::INFO, "game instantiated");
}

void Game::run() {
	if (!createWindow()) throw std::runtime_error("failed to create window");
	setupEventHandlers();

	while (!glfwWindowShouldClose(window)) {
		glClearColor(1.0, 0.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Game::handleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void Game::handleMouseMoveEvent(GLFWwindow* window, double x, double y) {
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	game->logger.log(LogLevel::INFO, std::string("Mouse moved to ") + std::to_string(x) + ", " + std::to_string(y));
}

void Game::setupEventHandlers() {
	glfwSetWindowUserPointer(window, this);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, handleKeyEvent);
	glfwSetCursorPosCallback(window, handleMouseMoveEvent);
}

bool Game::createWindow() {
	if (!glfwInit()) {
		logger.log(LogLevel::CRITICAL, "failed to initialize glfw");
		return false;
	}

	int windowSizeX = DEFAULT_WINDOW_SIZE_X;
	int windowSizeY = DEFAULT_WINDOW_SIZE_Y;

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	if (primaryMonitor == nullptr) {
		logger.log(LogLevel::CRITICAL, "no primary monitor");
		return false;
	}

	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
	if (videoMode != nullptr) {
		windowSizeX = videoMode->width;
		windowSizeY = videoMode->height;

		logger.log(
			LogLevel::INFO,
			std::string("current video mode ")
			+ std::to_string(videoMode->width) + "x" + std::to_string(videoMode->height) + "@" + std::to_string(videoMode->refreshRate) + "Hz "
			+ "R" + std::to_string(videoMode->redBits) + "G" + std::to_string(videoMode->greenBits) + "B" + std::to_string(videoMode->blueBits)
		);
	} else {
		logger.log(
			LogLevel::INFO,
			std::string("failed to get current video mode, defaulting to ") + std::to_string(windowSizeX) + "x" + std::to_string(windowSizeY)
		);
	}

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(windowSizeX, windowSizeY, GAME_TITLE, primaryMonitor, nullptr);
	if (window == nullptr) {
		logger.log(LogLevel::CRITICAL, "failed to create window");
		return false;
	}

	glfwMakeContextCurrent(window);
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		logger.log(LogLevel::CRITICAL, "failed to create opengl context");
		return false;
	} else {
		logger.log(
			LogLevel::INFO,
			std::string("created OpenGL ")
		    + std::to_string(GLAD_VERSION_MAJOR(version)) + "." + std::to_string(GLAD_VERSION_MINOR(version))
			+ " core context"
		);
	}
}

void Game::destroyWindow() {
	glfwMakeContextCurrent(nullptr);
	glfwTerminate();
	logger.log(LogLevel::INFO, "window destroyed");
}

Game::~Game() {
	destroyWindow();
	logger.log(LogLevel::INFO, "game destroyed");
}