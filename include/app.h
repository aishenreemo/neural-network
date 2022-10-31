#ifndef __APP_H__
#define __APP_H__

#include <libcollections/vector.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <pthread.h>

#include "neunet.h"

typedef struct __neunet_t__ neunet_t;

// MACROS

#define APP_WARN		"\033[1;33mwarn\033[0m: "
#define APP_FATAL		"\033[1;31mfatal\033[0m: "
#define APP_WARN_FN(FUNC)	"\033[1;33mwarn(" FUNC ")\033[0m: "
#define APP_FATAL_FN(FUNC)	"\033[1;31mfatal(" FUNC ")\033[0m: "

// STRUCTS

// main application struct
struct __app_t__ {
	bool		running;

	neunet_t	neunet;

	pthread_mutex_t	event_lock;
	pthread_t	event_thread;

	vector_t	sdl_event_queue;
	vector_t	command_queue;

	SDL_Renderer	*renderer;
	SDL_Window	*window;
};

// ENUMERATIONS

// pass this enum in app_quit
enum __app_exit_code_t__ {
	// exit program without error
	APP_EXIT_SUCCESS = 0,
	// exit program with error
	APP_EXIT_FAILURE = 1,
	// dont exit program
	APP_EXIT_IGNORE = -1,
};

enum __app_command_t__ {
	COMMAND_QUIT,
};

// PUBLIC FUNCTIONS

/**
 * initialization function for the application
 * only once (pre-allocate RAM).
 */
void app_init();

/**
 * quit the program (de-allocate RAM).
 *
 * - app_exit_code_t: if not less than
 *   zero, terminate the program with this
 *   as exit code.
 */
void app_quit(enum __app_exit_code_t__);

/**
 * listens for and converts any received SDL
 * events into commands.
 */
void app_listen();

/**
 * performs each command in the queue and
 * changes the program until the queue is empty.
 */
void app_update();

/**
 * use SDL rendering functions to draw stuff
 * on the screen based on data from the application.
 */
void app_render();

// GLOBAL VARIABLES

extern struct __app_t__ app;

#endif // __APP_H__
