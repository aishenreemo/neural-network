#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL.h>

#include <libcollections/vector.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "application.h"
#include "draw.h"

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

	int screen_width;
	int screen_height;
	SDL_GetWindowSize(app.window, &screen_width, &screen_height);

	SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255);

	uint layer_len = app.layer_size_vec.length;

	uint neuron_width = screen_width / 20;
	uint neuron_height = screen_height / 15;

	uint neuron_radius;
	neuron_radius = neuron_width < neuron_height ? neuron_width : neuron_height;
	neuron_radius = neuron_radius > 30 ? neuron_radius : 30;

	int neuron_h_distance = screen_width / (1 + layer_len);

	// draw neurons
	for (uint i = 0; i < app.layer_size_vec.length; i++) {
		uint *layer_size = vector_get(&app.layer_size_vec, i, NULL);
		int x = neuron_h_distance * (i + 1);

		int neuron_v_distance = screen_height / (1 + *layer_size);

		for (uint j = 0; j < *layer_size; j++) {
			int y = neuron_v_distance * (j + 1);
			draw_circle(x, y, neuron_radius);
		}
	}

	// draw weights
	for (uint i = 0; i + 1 < app.layer_size_vec.length; i++) {
		uint *layer_size_input = vector_get(&app.layer_size_vec, i + 0, NULL);
		uint *layer_size_output = vector_get(&app.layer_size_vec, i + 1, NULL);

		int x_input = neuron_h_distance * (i + 1);
		int x_output = neuron_h_distance * (i + 2);
		int neuron_v_distance_input = screen_height / (1 + *layer_size_input);
		int neuron_v_distance_output = screen_height / (1 + *layer_size_output);

		for (uint j = 0; j < (*layer_size_input) * (*layer_size_output); j++) {
			uint ii = j / (*layer_size_output);
			uint jj = j % (*layer_size_output);

			int x1 = x_input + neuron_radius;
			int y1 = neuron_v_distance_input * (ii + 1);
			int x2 = x_output - neuron_radius;
			int y2 = neuron_v_distance_output * (jj + 1);

			SDL_RenderDrawLine(app.renderer, x1, y1, x2, y2);
		}
	}

	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);

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
