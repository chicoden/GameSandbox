#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstdlib>
#include <string>

#include "Logger.hpp"

glm::mat4 infinitePerspectiveFocalLengthReverseZ(double focalLength, double aspectRatio, double zNear) {
	glm::mat4 result = glm::mat4();
	result[0][0] = focalLength / aspectRatio;
	result[1][1] = focalLength;
	result[3][2] = zNear;
	result[2][3] = -1.0;
	return result;
}

void handleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

int main() {
	gsbox::Logger logger("log.txt");

	if (!glfwInit()) {
		logger.log(gsbox::LogLevel::CRITICAL, "failed to initialize glfw");
		return EXIT_FAILURE;
	}

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	if (!primaryMonitor) {
		logger.log(gsbox::LogLevel::CRITICAL, "no primary monitor");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
	if (!videoMode) {
		logger.log(gsbox::LogLevel::CRITICAL, "failed to get current video mode");
		glfwTerminate();
		return EXIT_FAILURE;
	} else {
		logger.log(gsbox::LogLevel::INFO, std::string("current video mode ")
			+ std::to_string(videoMode->width) + "x" + std::to_string(videoMode->height) + "@" + std::to_string(videoMode->refreshRate) + "Hz "
			+ "R" + std::to_string(videoMode->redBits) + "G" + std::to_string(videoMode->greenBits) + "B" + std::to_string(videoMode->blueBits)
		);
	}

	GLFWwindow* window = glfwCreateWindow(videoMode->width, videoMode->height, "Game Sandbox", primaryMonitor, nullptr);
	if (!window) {
		logger.log(gsbox::LogLevel::CRITICAL, "failed to create window");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		logger.log(gsbox::LogLevel::CRITICAL, "failed to create OpenGL context");
		glfwMakeContextCurrent(nullptr);
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	} else {
		int majorVersion = GLAD_VERSION_MAJOR(version);
		int minorVersion = GLAD_VERSION_MINOR(version);
		logger.log(gsbox::LogLevel::INFO, std::string("created OpenGL ") + std::to_string(majorVersion) + "." + std::to_string(minorVersion) + " core context");
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	} else {
		logger.log(gsbox::LogLevel::WARNING, "raw mouse motion not supported, please polyfill");
	}

	glfwSetKeyCallback(window, handleKeyEvent);

	/// Quick tests
	glm::mat4 projMatrix = infinitePerspectiveFocalLengthReverseZ(1.0, static_cast<double>(videoMode->width) / videoMode->height, 0.01);

	int width, height, channels;
	unsigned char* image = stbi_load("assets/simple_sky/+x.png", &width, &height, &channels, 0);
	if (!image) {
		logger.log(gsbox::LogLevel::ERROR, "failed to load image");
		glfwMakeContextCurrent(nullptr);
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	logger.log(gsbox::LogLevel::INFO, std::string("opened image ") + std::to_string(width) + "x" + std::to_string(height) + "x" + std::to_string(channels));
	stbi_image_free(image);
	///

	while (!glfwWindowShouldClose(window)) {
		glClearColor(1.0, 0.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		//

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwMakeContextCurrent(nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}