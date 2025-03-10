#include <glad/gl.h>
#include <fstream>
#include <sstream>
#include <vector>
#include "Game.hpp"

using namespace sandbox;

Game::Game() : logger("log.txt"), window(nullptr) {
	logger.log(LogLevel::INFO, "game instantiated");
}

bool Game::run() {
	if (!createWindow()) return false;
	setupEventHandlers();

	if (compileProgram({
		{"shaders/simple_transform.vert", GL_VERTEX_SHADER},
		{"shaders/phong_light.frag", GL_FRAGMENT_SHADER}
	}) == 0) return false;

	while (!glfwWindowShouldClose(window)) {
		glClearColor(1.0, 1.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return true;
}

Game::~Game() {
	destroyWindow();
	logger.log(LogLevel::INFO, "game destroyed");
}

void Game::handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void Game::handleMousePosition(GLFWwindow* window, double x, double y) {
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	game->logger.log(LogLevel::INFO, std::string("Mouse moved to ") + std::to_string(x) + ", " + std::to_string(y));
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

	window = glfwCreateWindow(windowSizeX, windowSizeY, GAME_TITLE.c_str(), primaryMonitor, nullptr);
	if (window == nullptr) {
		logger.log(LogLevel::CRITICAL, "failed to create window");
		return false;
	}

	glfwMakeContextCurrent(window);
	int version = gladLoadGL(glfwGetProcAddress);
	if (version != 0) {
		logger.log(
			LogLevel::INFO,
			std::string("created opengl ")
			+ std::to_string(GLAD_VERSION_MAJOR(version)) + "." + std::to_string(GLAD_VERSION_MINOR(version))
			+ " core context"
		);
	} else {
		logger.log(LogLevel::CRITICAL, "failed to create opengl context");
		return false;
	}

	return true;
}

void Game::destroyWindow() {
	glfwMakeContextCurrent(nullptr);
	glfwTerminate();
	logger.log(LogLevel::INFO, "terminated glfw, any remaining glfw resources destroyed");
}

void Game::setupEventHandlers() {
	glfwSetWindowUserPointer(window, this);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, handleMousePosition);
	glfwSetKeyCallback(window, handleKey);
}

unsigned int Game::compileProgram(std::initializer_list<std::pair<fs::path, GLenum>> sources) {
	std::vector<unsigned int> shaders;
	shaders.reserve(sources.size());
	for (const auto& [path, type] : sources) {
		std::ifstream file(path);
		if (!file.is_open()) {
			logger.log(LogLevel::CRITICAL, std::string("failed to open shader source file ") + path.string());
			continue; // We don't want to stop here since we'd like debug info for all shaders, this is as far as we can go for this one though
		}

		std::ostringstream stream;
		stream << file.rdbuf();
		std::string_view view = stream.view();
		const char* code = view.data();
		int length = static_cast<int>(view.size());

		unsigned int shader = glCreateShader(type);
		glShaderSource(shader, 1, &code, &length);
		glCompileShader(shader);

		int compileStatus;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus == GL_FALSE) {
			logger.log(LogLevel::CRITICAL, std::string("failed to compile shader ") + path.string());

			int logLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
			char* infoLog = new char[logLength];
			glGetShaderInfoLog(shader, logLength, nullptr, infoLog);
			logger.log(LogLevel::DEBUG, infoLog);
			delete[] infoLog;

			glDeleteShader(shader);
			continue;
		}

		shaders.push_back(shader);
	}

	if (shaders.size() != sources.size()) {
		for (unsigned int shader : shaders) glDeleteShader(shader);
		return 0;
	}

	unsigned int program = glCreateProgram();
	for (unsigned int shader : shaders) glAttachShader(program, shader);
	glLinkProgram(program);

	for (unsigned int shader : shaders) {
		glDetachShader(program, shader);
		glDeleteShader(shader);
	}

	int linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		logger.log(LogLevel::CRITICAL, "failed to link program");
		glDeleteProgram(program);
		return 0;
	}

	return program;
}