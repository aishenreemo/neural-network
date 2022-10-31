#include "app.h"

void app_update() {
	// check if app is not running
	if (!app.running) {
		printf(APP_FATAL_FN("app_update") "no 'app' instance running. aborting.");
		exit(APP_EXIT_FAILURE);
	}

	// process every command in the command_queue vector (treating it also as a queue)
	while (app.command_queue.length > 0) {
		enum __app_command_t__ *cmd = vector_get(&app.command_queue, 0, NULL);

		if (COMMAND_QUIT == *cmd) app_quit(APP_EXIT_SUCCESS);

		// remove the first item then process the next one
		vector_remove(&app.command_queue, 0, NULL);
	}
}
