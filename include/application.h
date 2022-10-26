#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <libcollections/vector.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include <stdbool.h>

#include "neural_network.h"

// structs/enums
struct __application_t__ {
	bool running;

	neural_network_t network;
	vector_t layer_size_vec;

	pthread_mutex_t event_lock;
	pthread_t event_thread;

	vector_t sdl_event_pump;
	vector_t event_pump;

	SDL_Renderer *renderer;
	SDL_Window *window;
};

enum __event_t__ {
	EVENT_QUIT,
};

// aliases
typedef struct __application_t__ application_t;
typedef enum __event_t__ event_t;

// public fn decl
void app_init();
void app_quit(int);
void app_listen();
void app_update();
void app_render();

// global variables
extern application_t app;

#endif // __APPLICATION_H__
