#include <libcollections/vector.h>
#include <SDL2/SDL.h>
#include <pthread.h>

#include "app.h"

void app_on_event(SDL_Event *event);
void app_on_keydown(SDL_Event *event);

void app_listen() {
	// check if app is not running
	if (!app.running) {
		printf(APP_FATAL_FN("app_listen") "no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
		return;
	}

	pthread_mutex_lock(&app.event_lock);

	// process every events in the sdl_event_queue vector (treating it as a queue)
	while (app.sdl_event_queue.length > 0) {
		SDL_Event *event = vector_get(&app.sdl_event_queue, 0, NULL);

		// transform SDL event to a command
		app_on_event(event);

		// remove the first item then process the next one
		vector_remove(&app.sdl_event_queue, 0, NULL);
	}

	pthread_mutex_unlock(&app.event_lock);
}

void app_on_event(SDL_Event *event) {
	// transform SDL event to command
	// check if app is not running
	if (!app.running) {
		printf(APP_FATAL_FN("app_on_event") "no 'app' instance running. aborting.");
		exit(APP_EXIT_FAILURE);
	}

	if (event->type == SDL_QUIT) {
		// if client exit (forced quit) the program
		enum __app_command_t__ *cmd = malloc(sizeof(enum __app_command_t__));
		*cmd = COMMAND_QUIT;
		vector_push(&app.command_queue, cmd, NULL);
	} else if (event->type == SDL_KEYDOWN) {
		// if client press a key in the program
		app_on_keydown(event);
	}
}

void app_on_keydown(SDL_Event *event) {
	// if client press a key in the program
	if (!app.running) { // check if app is not running
		printf(APP_FATAL_FN("app_on_keydown") "no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	} else if (event->type != SDL_KEYDOWN) { // check if event type is a key down
		printf(APP_WARN_FN("app_on_keydown)") "event is not a keydown type. skipping");
		return;
	}

	SDL_Keymod keymod = event->key.keysym.mod;
	SDL_Keycode keycode = event->key.keysym.sym;

	if (keycode == SDLK_c && (keymod & KMOD_CTRL) != 0) {
		// if client press ctrl + c
		enum __app_command_t__ *cmd = malloc(sizeof(enum __app_command_t__));
		*cmd = COMMAND_QUIT;
		vector_push(&app.command_queue, cmd, NULL);
	}
}
