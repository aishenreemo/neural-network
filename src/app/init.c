#include <libcollections/vector.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "neunet.h"
#include "app.h"

struct __app_t__ app;

void *app_event_thread(void *);

void app_init() {
	if (app.running) {
		printf(APP_WARN_FN("app_init") "cannot initialize 'app' twice. skipping\n");
		return;
	}

	srand((uint) time(NULL));

	// :)
	app.running = true;

	// initialize vectors
	vector_init(&app.sdl_event_queue, NULL);
	vector_init(&app.command_queue, NULL);

	// initialize neural network
	vector_t layer_sizes;
	vector_init(&layer_sizes, NULL);

	uint *ptr;
	ptr = malloc(sizeof(uint)); *ptr = 2; vector_push(&layer_sizes, ptr, NULL);
	ptr = malloc(sizeof(uint)); *ptr = 3; vector_push(&layer_sizes, ptr, NULL);
	ptr = malloc(sizeof(uint)); *ptr = 2; vector_push(&layer_sizes, ptr, NULL);

	neunet_init(&app.neunet, &layer_sizes);
	neunet_randomize_weights(&app.neunet);

	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf(APP_FATAL "error initializing SDL... %s\n", SDL_GetError());
		app_quit(APP_EXIT_FAILURE);
	}

	// window
	app.window = SDL_CreateWindow(
		"Neural Network Test",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                800, 600,
		SDL_WINDOW_RESIZABLE
	);

	// renderer
	app.renderer = SDL_CreateRenderer(
		app.window,
		-1,
		SDL_RENDERER_ACCELERATED
	);

	// initialize threads
	pthread_create(&app.event_thread, NULL, app_event_thread, NULL);
}

void app_quit(enum __app_exit_code_t__ exit_code) {
	// check if app is not running
	if (!app.running) {
		printf(APP_FATAL_FN("app_quit") "no 'app' instance running. aborting.\n");
		exit(APP_EXIT_FAILURE);
	}

	// :(
	app.running = false;

	// cancel threads
	pthread_cancel(app.event_thread);

	// deallocate neural network
	neunet_drop(&app.neunet);

	// deallocate vectors
	vector_drop(&app.sdl_event_queue);
	vector_drop(&app.command_queue);

	// deallocate SDL
	SDL_DestroyRenderer(app.renderer);
	SDL_DestroyWindow(app.window);

	// abort program or don't
	if ((int) exit_code > APP_EXIT_IGNORE) {
		printf("exiting....\n");
		exit((int) exit_code);
	}
}

void *app_event_thread(void *_) {
	// separate thread that accept events
	// check if app is not running
	if (!app.running) {
		printf(
			APP_FATAL_FN("app_event_thread")
			"no 'app' instance running. aborting."
		);
		exit(APP_EXIT_FAILURE);
	}

	while (true) {
		SDL_Event *event = malloc(sizeof(SDL_Event));
		SDL_WaitEvent(event); // wait for an event

		// push that event to a queue where `app_listen` reads it
		pthread_mutex_lock(&app.event_lock);
		vector_push(&app.sdl_event_queue, event, NULL);
		pthread_mutex_unlock(&app.event_lock);
	}

	return NULL;
}
