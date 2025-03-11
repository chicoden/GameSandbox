#include <cstdlib>
#include "Game.hpp"

int main() {
	sandbox::Game game;

	if (game.init()) {
		game.run();
	} else {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}