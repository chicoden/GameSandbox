#pragma once

//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#include <spdlog/spdlog.h>

#include <GLFW/glfw3.h>
#include <string>

#define ENABLE_VALIDATION_LAYERS

namespace sandbox {
	static const std::string GAME_TITLE = "Game Sandbox";
	static const int DEFAULT_WINDOW_SIZE_X = 800;
	static const int DEFAULT_WINDOW_SIZE_Y = 600;

	const double NEAR_CLIP_DISTANCE = 0.1;
	const double CAMERA_FOCAL_LENGTH = 2.0;

	const double CAMERA_ROTATION_SPEED = 4.0;
	const double CAMERA_PLAYER_DISTANCE = 3.0;

	const double PLAYER_WALK_SPEED = 0.005;
	const double PLAYER_TURN_SPEED = 0.004;

	const int KEY_EXIT_FULLSCREEN = GLFW_KEY_ESCAPE;
	const int KEY_MOVE_FORWARD    = GLFW_KEY_W;
	const int KEY_TURN_LEFT       = GLFW_KEY_A;
	const int KEY_MOVE_BACKWARD   = GLFW_KEY_S;
	const int KEY_TURN_RIGHT      = GLFW_KEY_D;
}