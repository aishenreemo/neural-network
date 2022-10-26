#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "neural_network.h"


// private function declarations
void neural_network_layer_init(neural_network_layer_t *, uint, uint);
void neural_network_layer_calc_outputs(neural_network_layer_t *, vector_t *, vector_t *);

// public function definitions
void neural_network_init(neural_network_t *neural_network, vector_t *layer_size_vec) {
	srand((uint) time(NULL));

	vector_init(&neural_network->layer_vec, NULL);

	for (uint i = 0; (i + 1) < layer_size_vec->length; i++) {
		uint *input_len = vector_get(layer_size_vec, i, NULL);
		uint *output_len = vector_get(layer_size_vec, i + 1, NULL);

		neural_network_layer_t *layer = malloc(sizeof(neural_network_layer_t));
		neural_network_layer_init(layer, *input_len, *output_len);

		vector_push(&neural_network->layer_vec, layer, NULL);
	}
}

void neural_network_drop(neural_network_t *neural_network) {
	for (uint i = 0; i < neural_network->layer_vec.length; i++) {
		neural_network_layer_t *layer = vector_get(&neural_network->layer_vec, i, NULL);

		vector_drop(&layer->weight_vec);
		vector_drop(&layer->bias_vec);
	}

	vector_drop(&neural_network->layer_vec);
}

void neural_network_calc_outputs(
	neural_network_t *neural_network,
	vector_t *input_vec,
	vector_t *output_vec
) {
	vector_t mutable_vec;
	vector_init(&mutable_vec, NULL);

	vector_copy(&mutable_vec, input_vec, sizeof(double), NULL);

	for (uint i = 0; i < neural_network->layer_vec.length; i++) {
		neural_network_layer_t *layer = vector_get(&neural_network->layer_vec, i, NULL);

		// init input and output vectors
		vector_t input_layer_vec;
		vector_t output_layer_vec;
		vector_init(&input_layer_vec, NULL);
		vector_init(&output_layer_vec, NULL);

		vector_copy(&input_layer_vec, &mutable_vec, sizeof(double), NULL);
		neural_network_layer_calc_outputs(layer, &input_layer_vec, &output_layer_vec);
		vector_copy(&mutable_vec, &output_layer_vec, sizeof(double), NULL);

		// drop vectors in this scope
		vector_drop(&input_layer_vec);
		vector_drop(&output_layer_vec);
	}

	vector_copy(output_vec, &mutable_vec, sizeof(double), NULL);

	vector_drop(&mutable_vec);
}

void neural_network_randomize_weights(neural_network_t *neural_network) {
	double height = NEURAL_NETWORK_WEIGHT_AMPLITUDE * 2;

	for (uint i = 0; i < neural_network->layer_vec.length; i++) {
		neural_network_layer_t *layer = vector_get(&neural_network->layer_vec, i, NULL);

		for (uint j = 0; j < layer->weight_vec.length; j++) {
			double *val = vector_get(&layer->weight_vec, j, NULL);
			double rand_val;
			rand_val = (double) rand();
			rand_val /= (double) RAND_MAX;
			rand_val *= height;
			rand_val -= NEURAL_NETWORK_WEIGHT_AMPLITUDE;

			*val = rand_val;
		}
	}
}

// private function definitions
void neural_network_layer_init(
	neural_network_layer_t *layer,
	uint input_len,
	uint output_len
) {
	layer->input_len = input_len;
	layer->output_len = output_len;

	// initialize weights
	vector_init(&layer->weight_vec, NULL);
	for (uint i = 0; i < input_len * output_len; i++) {
		double *item = malloc(sizeof(double)); *item = 0.0;
		vector_push(&layer->weight_vec, item, NULL);
	}

	// initialize biases
	vector_init(&layer->bias_vec, NULL);
	for (uint i = 0; i < output_len; i++) {
		double *item = malloc(sizeof(double)); *item = 0.0;
		vector_push(&layer->bias_vec, item, NULL);
	}
}

void neural_network_layer_calc_outputs(
	neural_network_layer_t *layer,
	vector_t *input_vec,
	vector_t *output_vec
) {
	for (uint i = 0; i < layer->output_len; i++) {
		double *weighted_input = vector_get(&layer->bias_vec, i, NULL);

		for (uint j = 0; j < layer->input_len; j++) {
			uint weight_index = (layer->input_len * i) + j;
			double *input = vector_get(input_vec, j, NULL);
			double *weight = vector_get(&layer->weight_vec, weight_index, NULL);

			*weighted_input += (*input) * (*weight);
		}

		double *item = malloc(sizeof(double)); *item = *weighted_input;
		vector_push(output_vec, item, NULL);
	}
}
