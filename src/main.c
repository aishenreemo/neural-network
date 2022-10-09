#include <SDL2/SDL_timer.h>
#include <stdlib.h>

#include "application.h"

int main() {
	app_init();

	while (true) {
		app_listen();
		app_update();
		app_render();
		SDL_Delay(1000 / 30);
	}

	app_quit();

	return EXIT_SUCCESS;
}
