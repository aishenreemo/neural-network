#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL.h>

#include <libcollections/vector.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "application.h"
#include "render.h"

// global variables
application_t app;

// private fn declarations
void *app_event_thread(void *);
void app_on_event(SDL_Event *event);
void app_on_keydown(SDL_Event *event);

void app_init() {
	// check if app is already running
	if (app.running) {
		printf("warn(app_init): cannot initialize 'app' twice. skipping");
		return;
	}

	// :)
	app.running = true;

	// initialize vectors
	vector_init(&app.layer_size_vec, NULL);
	vector_init(&app.sdl_event_pump, NULL);
	vector_init(&app.event_pump, NULL);

	// initialize neural network
	uint *lsv_item;
	lsv_item = malloc(sizeof(uint)); *lsv_item = 2; vector_push(&app.layer_size_vec, lsv_item, NULL);
	lsv_item = malloc(sizeof(uint)); *lsv_item = 3; vector_push(&app.layer_size_vec, lsv_item, NULL);
	lsv_item = malloc(sizeof(uint)); *lsv_item = 2; vector_push(&app.layer_size_vec, lsv_item, NULL);

	neural_network_init(&app.network, &app.layer_size_vec);

	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
		app_quit();
	}

	app.window = SDL_CreateWindow("Neural Network Test",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                800, 600,
		SDL_WINDOW_RESIZABLE
	);
	app.renderer = SDL_CreateRenderer(
		app.window,
		-1,
		SDL_RENDERER_ACCELERATED
	);

	// initialize threads
	pthread_create(&app.event_thread, NULL, app_event_thread, NULL);
}

void app_quit() {
	// check if app is not running
	if (!app.running) {
		printf("fatal(app_quit): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	// dealloc memory for neural network
	neural_network_drop(&app.network);

	// :(
	app.running = false;

	// deallocate vectors
	vector_drop(&app.layer_size_vec);
	vector_drop(&app.sdl_event_pump);
	vector_drop(&app.event_pump);

	// cancel threads
	pthread_cancel(app.event_thread);

	// deallocate SDL
	SDL_DestroyRenderer(app.renderer);
	SDL_DestroyWindow(app.window);

	// abort program
	printf("exiting....\n");
	exit(EXIT_SUCCESS);
}

void app_listen() {
	// check if app is not running
	if (!app.running) {
		printf("fatal(app_listen): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
		return;
	}

	pthread_mutex_lock(&app.event_lock);

	// process every events in the sdl_event_pump vector (treating it as a queue)
	while (app.sdl_event_pump.length > 0) {
		SDL_Event *event = vector_get(&app.sdl_event_pump, 0, NULL);

		// transform SDL event to a command
		app_on_event(event);

		// remove the first item then process the next one
		vector_remove(&app.sdl_event_pump, 0, NULL);
	}

	pthread_mutex_unlock(&app.event_lock);
}

void app_update() {
	// check if app is not running
	if (!app.running) {
		printf("fatal(app_update): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	// process every command in the event_pump vector (treating it also as a queue)
	while (app.event_pump.length > 0) {
		event_t *event = vector_get(&app.event_pump, 0, NULL);
		switch (*event) {

		// if command tells the program to abort
		case EVENT_QUIT:
			app_quit();
			break; // unreachable
		}

		// remove the first item then process the next one
		vector_remove(&app.event_pump, 0, NULL);
	}
}

// private function implementations
void *app_event_thread(void *_) {
	// separate thread that accept events
	// check if app is not running
	if (!app.running) {
		printf("fatal(app_event_thread): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	while (app.running) {
		SDL_Event *event = malloc(sizeof(SDL_Event));
		SDL_WaitEvent(event); // wait for an event

		// push that event to a queue where `app_listen` reads it
		pthread_mutex_lock(&app.event_lock);
		vector_push(&app.sdl_event_pump, event, NULL);
		pthread_mutex_unlock(&app.event_lock);
	}

	return NULL;
}

void app_on_event(SDL_Event *event) {
	// transform SDL event to command
	// check if app is not running
	if (!app.running) {
		printf("fatal(app_on_keydown): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	if (event->type == SDL_QUIT) {
		// if client exit the program
		event_t *item = malloc(sizeof(event_t)); *item = EVENT_QUIT;
		vector_push(&app.event_pump, item, NULL);
	} else if (event->type == SDL_KEYDOWN) {
		// if client press a key in the program
		app_on_keydown(event);
	}
}

void app_on_keydown(SDL_Event *event) {
	// if client press a key in the program
	if (!app.running) { // check if app is not running
		printf("fatal(app_on_keydown): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	} else if (event->type != SDL_KEYDOWN) { // check if event type is a key down
		printf("warn(app_on_keydown): event is not a keydown type. skipping");
		return;
	}

	SDL_Keymod keymod = event->key.keysym.mod;
	SDL_Keycode keycode = event->key.keysym.sym;

	if (keycode == SDLK_c && (keymod & KMOD_CTRL) != 0) {
		// if client press ctrl + c
		// send a QUIT command
		event_t *item = malloc(sizeof(event_t)); *item = EVENT_QUIT;
		vector_push(&app.event_pump, item, NULL);
		return;
	}
}
