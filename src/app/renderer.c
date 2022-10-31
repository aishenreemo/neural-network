#include <SDL2/SDL.h>
#include <stdlib.h>

#include "app.h"
#include "neunet.h"

void draw_circle(int, int, uint);

void app_render() {
	if (!app.running) {
		printf(APP_FATAL_FN("app_render") "no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	// render gaem
	SDL_RenderClear(app.renderer);

	// get window size
	int screen_width;
	int screen_height;
	SDL_GetWindowSize(app.window, &screen_width, &screen_height);

	// default sdl flags
	SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255);
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);

	// number of layers in neural network
	uint layer_len = app.neunet.layer_sizes.length;

	// determine neuron size
	uint neuron_width = screen_width / 20;
	uint neuron_height = screen_height / 15;

	// determine neuron radius
	uint neuron_radius;
	neuron_radius = neuron_width < neuron_height ? neuron_width : neuron_height;
	neuron_radius = neuron_radius > 30 ? neuron_radius : 30;

	// horizontal distance between neurons
	int neuron_h_distance = screen_width / (1 + layer_len);

	// draw neurons
	for (uint i = 0; i < layer_len; i++) {
		uint *layer_size = vector_get(&app.neunet.layer_sizes, i, NULL);

		// x position of neuron
		int x = neuron_h_distance * (i + 1);

		// vertical distance between neurons
		int neuron_v_distance = screen_height / (1 + *layer_size);

		for (uint j = 0; j < *layer_size; j++) {
			// y position of neuron
			int y = neuron_v_distance * (j + 1);
			draw_circle(x, y, neuron_radius);
		}
	}

	// draw weights
	for (uint i = 0; i + 1 < layer_len; i++) {
		struct __neunet_layer_t__ *layer = vector_get(&app.neunet.layers, i, NULL);

		// how many neurons on previous layer & current layer
		uint *layer_size_input = vector_get(&app.neunet.layer_sizes, i + 0, NULL);
		uint *layer_size_output = vector_get(&app.neunet.layer_sizes, i + 1, NULL);

		// x position of weight line in previous/current layer
		int x_input = neuron_h_distance * (i + 1);
		int x_output = neuron_h_distance * (i + 2);

		// vertical distance between neurons for previous/current layer
		int neuron_v_distance_input = screen_height / (1 + *layer_size_input);
		int neuron_v_distance_output = screen_height / (1 + *layer_size_output);

		// for each neuron pair in previous and current layer
		for (uint j = 0; j < (*layer_size_input) * (*layer_size_output); j++) {
			uint ii = j / (*layer_size_output);
			uint jj = j % (*layer_size_output);

			// line position
			int x1 = x_input + neuron_radius;
			int y1 = neuron_v_distance_input * (ii + 1);
			int x2 = x_output - neuron_radius;
			int y2 = neuron_v_distance_output * (jj + 1);

			double *weight_val = vector_get(&layer->weights, j, NULL);

			// color values
			uint r = (*weight_val < 0) ? 255 : 0;
			uint g = (*weight_val > 0) ? 255 : 0;
			uint b = 0;
			uint a = 255.0 * fabs(*weight_val) / NEUNET_AMPLITUDE;

			// draw that line
			SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
			SDL_RenderDrawLine(app.renderer, x1, y1, x2, y2);
		}
	}

	// set sdl flags
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);

	SDL_RenderPresent(app.renderer);
}

void draw_circle(int center_x, int center_y, uint radius) {
	uint diameter = (radius * 2);

	int x = radius - 1;
	int y = 0;

	int t_x = 1;
	int t_y = 1;

	int error = t_x - diameter;

	while (x >= y) {
		SDL_RenderDrawPoint(app.renderer, center_x + x, center_y - y);
		SDL_RenderDrawPoint(app.renderer, center_x + x, center_y + y);
		SDL_RenderDrawPoint(app.renderer, center_x - x, center_y - y);
		SDL_RenderDrawPoint(app.renderer, center_x - x, center_y + y);
		SDL_RenderDrawPoint(app.renderer, center_x + y, center_y - x);
		SDL_RenderDrawPoint(app.renderer, center_x + y, center_y + x);
		SDL_RenderDrawPoint(app.renderer, center_x - y, center_y - x);
		SDL_RenderDrawPoint(app.renderer, center_x - y, center_y + x);

		if (error <= 0) {
			y += 1;
			error += t_y;
			t_y += 2;
		}

		if (error > 0) {
			x -= 1;
			t_x += 2;
			error += t_x - diameter;
		}
	}
}
