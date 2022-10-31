#include <SDL2/SDL_timer.h>
#include <stdbool.h>

#include "app.h"

int main() {
	app_init();

	// infinite loop
	// only stops if you called the function app_quit()
	while (true) {
		app_listen();
		app_update();
		app_render();

		// sleep for (1s / 30ms)
		SDL_Delay(1000 / 30);
	}
}
