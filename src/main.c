#include <SDL2/SDL_timer.h>
#include <stdlib.h>

#include "application.h"

int main() {
	// initialize application (pre-allocs memory)
	app_init();

	// infinite loop
	// only stops if you called the function app_quit()
	while (true) {
		// listen to events
		// transforms any SDL events into our own instructions
		// e.g. client press CTRL-c -> APP_QUIT
		app_listen();
		// update the application
		// make changes by the instructions made by `app_listen`
		// e.g. APP_QUIT -> app_quit()
		app_update();
		// render the application
		// draw stuff on the screen based on the information of app
		app_render();
		// sleep for (1s / 30ms)
		SDL_Delay(1000 / 30);
	}

	// unreachable code

	// quit application (de-allocs used memory)
	app_quit(EXIT_SUCCESS);
}
