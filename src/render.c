#include <SDL2/SDL.h>
#include <stdlib.h>

#include "application.h"
#include "render.h"
#include "draw.h"

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
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);

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
		neural_network_layer_t *layer = vector_get(&app.network.layer_vec, i, NULL);
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

			double *weight_val = vector_get(&layer->weight_vec, j, NULL);

			uint r = (*weight_val < 0) ? 255 : 0;
			uint g = (*weight_val > 0) ? 255 : 0;
			uint b = 0;
			uint a = 255.0 * fabs(*weight_val) / NEURAL_NETWORK_WEIGHT_AMPLITUDE;
			SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
			SDL_RenderDrawLine(app.renderer, x1, y1, x2, y2);
		}
	}

	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);

	SDL_RenderPresent(app.renderer);
}
