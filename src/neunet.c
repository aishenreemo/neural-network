#include <libcollections/vector.h>
#include <time.h>

#include "neunet.h"

void neunet_layer_init(struct __neunet_layer_t__ *, uint, uint);
void neunet_layer_calculate_outputs(struct __neunet_layer_t__ *, vector_t *, vector_t *);

void neunet_init(struct __neunet_t__ *neunet, vector_t *layer_sizes) {
	vector_init(&neunet->layers, NULL);
	vector_init(&neunet->layer_sizes, NULL);

	vector_copy(&neunet->layer_sizes, layer_sizes, sizeof(uint), NULL);

	for (uint i = 0; (i + 1) < layer_sizes->length; i++) {
		uint *input_len  = vector_get(layer_sizes, i + 0, NULL);
		uint *output_len = vector_get(layer_sizes, i + 1, NULL);

		struct __neunet_layer_t__ *layer = malloc(sizeof(struct __neunet_layer_t__));
		neunet_layer_init(layer, *input_len, *output_len);

		vector_push(&neunet->layers, layer, NULL);
	}

	vector_drop(layer_sizes);
}

void neunet_drop(struct __neunet_t__ *neunet) {
	for (uint i = 0; i < neunet->layers.length; i++) {
		struct __neunet_layer_t__ *layer = vector_get(&neunet->layers, i, NULL);

		vector_drop(&layer->weights);
		vector_drop(&layer->biases);
	}

	vector_drop(&neunet->layer_sizes);
	vector_drop(&neunet->layers);
}

void neunet_calculate_outputs(
	struct __neunet_t__ *neunet,
	vector_t *input_vec,
	vector_t *output_vec
) {
	vector_t mutable_vec;
	vector_init(&mutable_vec, NULL);

	vector_copy(&mutable_vec, input_vec, sizeof(double), NULL);

	for (uint i = 0; i < neunet->layers.length; i++) {
		struct __neunet_layer_t__ *layer = vector_get(&neunet->layers, i, NULL);

		// init input and output vectors
		vector_t input_layer;
		vector_t output_layer;
		vector_init(&input_layer, NULL);
		vector_init(&output_layer, NULL);

		vector_copy(&input_layer, &mutable_vec, sizeof(double), NULL);
		neunet_layer_calculate_outputs(layer, &input_layer, &output_layer);
		vector_copy(&mutable_vec, &output_layer, sizeof(double), NULL);

		// drop vectors in this scope
		vector_drop(&input_layer);
		vector_drop(&output_layer);
	}

	vector_copy(output_vec, &mutable_vec, sizeof(double), NULL);

	vector_drop(&mutable_vec);
}

void neunet_randomize_weights(struct __neunet_t__ *neunet) {
	double height = NEUNET_AMPLITUDE * 2;

	for (uint i = 0; i < neunet->layers.length; i++) {
		struct __neunet_layer_t__ *layer = vector_get(&neunet->layers, i, NULL);

		for (uint j = 0; j < layer->weights.length; j++) {
			double *val = vector_get(&layer->weights, j, NULL);
			double rand_val;
			rand_val = (double) rand();
			rand_val /= (double) RAND_MAX;
			rand_val *= height;
			rand_val -= NEUNET_AMPLITUDE;

			*val = rand_val;
		}
	}
}

void neunet_layer_init(
	struct __neunet_layer_t__ *layer,
	uint input_len,
	uint output_len
) {
	layer->input_len = input_len;
	layer->output_len = output_len;

	// initialize weights
	vector_init(&layer->weights, NULL);
	for (uint i = 0; i < input_len * output_len; i++) {
		double *item = malloc(sizeof(double)); *item = 0.0;
		vector_push(&layer->weights, item, NULL);
	}

	// initialize biases
	vector_init(&layer->biases, NULL);
	for (uint i = 0; i < output_len; i++) {
		double *item = malloc(sizeof(double)); *item = 0.0;
		vector_push(&layer->biases, item, NULL);
	}

}

void neunet_layer_calculate_outputs(
	struct __neunet_layer_t__ *layer,
	vector_t *input_vec,
	vector_t *output_vec
) {
	for (uint i = 0; i < layer->output_len; i++) {
		double *weighted_input = vector_get(&layer->biases, i, NULL);

		for (uint j = 0; j < layer->input_len; j++) {
			uint weight_index = (layer->input_len * i) + j;
			double *input = vector_get(input_vec, j, NULL);
			double *weight = vector_get(&layer->weights, weight_index, NULL);

			*weighted_input += (*input) * (*weight);
		}

		double *item = malloc(sizeof(double)); *item = *weighted_input;
		vector_push(output_vec, item, NULL);
	}
}
