#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL.h>

#include <libcollections/vector.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "application.h"

// global vars
application_t app;

// private fn declarations
void *app_event_thread(void *);
void app_on_event(SDL_Event *event);
void app_on_keydown(SDL_Event *event);

void app_init() {
	if (app.running) {
		printf("warn(app_init): cannot initialize 'app' twice. skipping");
		return;
	}

	app.running = true;

	vector_init(&app.layer_size_vec, NULL);
	vector_init(&app.sdl_event_pump, NULL);
	vector_init(&app.event_pump, NULL);

	// layer sizes
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

	app.window = SDL_CreateWindow("GAME",
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

	pthread_create(&app.event_thread, NULL, app_event_thread, NULL);
}

void app_quit() {
	if (!app.running) {
		printf("fatal(app_quit): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	neural_network_drop(&app.network);

	app.running = false;

	vector_drop(&app.layer_size_vec);
	vector_drop(&app.sdl_event_pump);
	vector_drop(&app.event_pump);

	pthread_cancel(app.event_thread);

	SDL_DestroyRenderer(app.renderer);
	SDL_DestroyWindow(app.window);

	printf("exiting....\n");
	exit(EXIT_SUCCESS);
}

void app_listen() {
	if (!app.running) {
		printf("fatal(app_listen): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
		return;
	}

	pthread_mutex_lock(&app.event_lock);
	while (app.sdl_event_pump.length > 0) {
		SDL_Event *event = vector_get(&app.sdl_event_pump, 0, NULL);
		app_on_event(event);

		vector_remove(&app.sdl_event_pump, 0, NULL);
	}

	pthread_mutex_unlock(&app.event_lock);
}

void app_update() {
	if (!app.running) {
		printf("fatal(app_update): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	// make changes to the application based on the events
	while (app.event_pump.length > 0) {
		event_t *event = vector_get(&app.event_pump, 0, NULL);
		switch (*event) {

		case EVENT_QUIT:
			app_quit();
			break; // unreachable
		}

		vector_remove(&app.event_pump, 0, NULL);
	}
}

void app_render() {
	if (!app.running) {
		printf("fatal(app_render): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	// render gaem
	SDL_RenderClear(app.renderer);
	SDL_RenderPresent(app.renderer);
}

// private function implementations
void *app_event_thread(void *_) {
	if (!app.running) {
		printf("fatal(app_event_thread): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	// listen to events
	while (app.running) {
		SDL_Event *event = malloc(sizeof(SDL_Event));
		SDL_WaitEvent(event);
		pthread_mutex_lock(&app.event_lock);
		vector_push(&app.sdl_event_pump, event, NULL);
		pthread_mutex_unlock(&app.event_lock);
	}

	return NULL;
}

void app_on_event(SDL_Event *event) {
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
	if (!app.running) {
		printf("fatal(app_on_keydown): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	} else if (event->type != SDL_KEYDOWN) {
		printf("warn(app_on_keydown): event is not a keydown type. skipping");
		return;
	}

	SDL_Keymod keymod = event->key.keysym.mod;
	SDL_Keycode keycode = event->key.keysym.sym;

	if (keycode == SDLK_c && (keymod & KMOD_CTRL) != 0) {
		// if client press ctrl + c
		event_t *item = malloc(sizeof(event_t)); *item = EVENT_QUIT;
		vector_push(&app.event_pump, item, NULL);
		return;
	}
}
