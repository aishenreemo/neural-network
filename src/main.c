#include <unistd.h>
#include <stdlib.h>

#include "application.h"

int main() {
	app_init();

	while (true) {
		app_listen();
		app_update();
		app_render();
		usleep(1000 / 30); // 30 fps (not exact)
	}

	app_quit();

	return EXIT_SUCCESS;
}
