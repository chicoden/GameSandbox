#include <cstdlib>
#include "Game.hpp"

int main() {
	sandbox::Game game;
	if (!game.run()) return EXIT_FAILURE;
	return EXIT_SUCCESS;
}