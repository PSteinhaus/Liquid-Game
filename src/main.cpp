#include "main.hpp"

Engine engine {};

int main() {
	while ( !TCODConsole::isWindowClosed() && engine.gameStatus!=Engine::EXIT ) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
	TCOD_quit();
	return 0;
}