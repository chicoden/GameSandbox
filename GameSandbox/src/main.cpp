#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "Game.hpp"

int main() {
	try {
		gsbox::Game game;
		game.run();
	} catch (const std::exception& error) {
		std::cout << error.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}