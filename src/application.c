#include <libcollections/vector.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ncurses_defs.h"
#include "application.h"

// global vars
application_t app;

// private fn declarations
void *app_event_thread(void *);

void app_init() {
	if (app.running) {
		printf("warn(app_init): cannot initialize 'app' twice. skipping");
		return;
	}

	app.running = true;

	// initialize ncurses
	ncurses_init_screen();
	ncurses_cursor_visibility(0);
	ncurses_keypad(stdscr, true);
	ncurses_raw_mode();
	ncurses_no_echo();
	ncurses_clear();

	if (ncurses_has_colors()) {
		ncurses_use_default_colors();
		ncurses_start_color();
	}

	vector_init(&app.keypress_queue, NULL);
	vector_init(&app.layer_size_vec, NULL);
	vector_init(&app.event_pump, NULL);

	// layer sizes
	uint *lsv_item;
	lsv_item = malloc(sizeof(uint)); *lsv_item = 2; vector_push(&app.layer_size_vec, lsv_item, NULL);
	lsv_item = malloc(sizeof(uint)); *lsv_item = 3; vector_push(&app.layer_size_vec, lsv_item, NULL);
	lsv_item = malloc(sizeof(uint)); *lsv_item = 2; vector_push(&app.layer_size_vec, lsv_item, NULL);

	neural_network_init(&app.network, &app.layer_size_vec);

	pthread_create(&app.event_thread, NULL, app_event_thread, NULL);
}

void app_quit() {
	if (!app.running) {
		printf("fatal(app_quit): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	neural_network_drop(&app.network);
	ncurses_endwin();

	app.running = false;

	vector_drop(&app.layer_size_vec);
	vector_drop(&app.keypress_queue);
	vector_drop(&app.event_pump);

	pthread_cancel(app.event_thread);
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
	while (app.keypress_queue.length > 0) {
		switch (*((int *) vector_get(&app.keypress_queue, 0, NULL))) {

		// if client pressed ctrl c
		case 'c' & 037:;
			event_t *item = malloc(sizeof(event_t)); *item = EVENT_QUIT;
			vector_push(&app.event_pump, item, NULL);
			break;
		}

		vector_remove(&app.keypress_queue, 0, NULL);
	}
	pthread_mutex_unlock(&app.event_lock);
}

void app_update() {
	if (!app.running) {
		printf("fatal(app_update): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	while (app.event_pump.length > 0) {
		switch (*((event_t *) vector_get(&app.event_pump, 0, NULL))) {

		case EVENT_QUIT:
			app_quit();
			break; // unreachable
		}

		vector_remove(&app.keypress_queue, 0, NULL);
	}
}

void app_render() {
	if (!app.running) {
		printf("fatal(app_render): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	ncurses_clear_nr();

	uint screen_width = COLS;
	uint screen_height = LINES;
	uint neuron_height = 3;
	uint neuron_length = 7;
	uint layer_len = app.layer_size_vec.length;
	uint neuron_h_distance = screen_width / (1 + layer_len);

	for (uint i = 0; i < layer_len; i++) {
		uint x = neuron_h_distance * (i + 1);
		uint *layer_size = vector_get(&app.layer_size_vec, i, NULL);
		uint neuron_v_distance = screen_height / (1 + *layer_size);

		for (uint j = 0; j < *layer_size; j++) {
			uint y = neuron_v_distance * (j + 1);
			ncurses_move_addch(x, y, 'O');
			ncurses_move_hline(x - 3, y - 1, 0, neuron_length);
			ncurses_move_hline(x - 3, y + 1, 0, neuron_length);
			ncurses_move_vline(x - 3, y - 1, 0, neuron_height);
			ncurses_move_vline(x + 3, y - 1, 0, neuron_height);
			ncurses_move_addch(x - 3, y - 1, ACS_ULCORNER);
			ncurses_move_addch(x + 3, y - 1, ACS_URCORNER);
			ncurses_move_addch(x - 3, y + 1, ACS_LLCORNER);
			ncurses_move_addch(x + 3, y + 1, ACS_LRCORNER);
		}
	}

	// refresh
	ncurses_refresh();
}

// private function implementations
void *app_event_thread(void *_) {
	if (!app.running) {
		printf("fatal(app_event_thread): no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	while (true) {
		int *ch = malloc(sizeof(int)); *ch = getch();
		pthread_mutex_lock(&app.event_lock);
		vector_push(&app.keypress_queue, ch, NULL);
		pthread_mutex_unlock(&app.event_lock);
	}

	return NULL;
}
