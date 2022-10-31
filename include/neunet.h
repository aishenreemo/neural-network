#ifndef __NEUNET_H__
#define __NEUNET_H__

#include <libcollections/vector.h>
#include <stdlib.h>

// STRUCTS

/**
 * neural network struct,
 * consist of layers
 */
struct __neunet_t__ {
	vector_t	layers;
	vector_t	layer_sizes;
};

/**
 * neural network layer struct,
 *
 * consist of number of neurons (input & output),
 * weight and biases
 */
struct __neunet_layer_t__ {
	uint		input_len;
	uint		output_len;
	vector_t	weights;
	vector_t	biases;
};

// CONSTANTS

static const double NEUNET_AMPLITUDE = 1.0;

// PUBLIC FUNCTIONS

/**
 * initialize neural network
 *
 * parameters:
 * - neural network struct pointer
 * - layer sizes
 */
void neunet_init(struct __neunet_t__ *, vector_t *);
/**
 * deallocate neural network
 *
 * parameters:
 * - neural network struct pointer
 */
void neunet_drop(struct __neunet_t__ *);

/**
 * calculate outputs
 *
 * parameters:
 * - neural network struct pointer
 * - input vector pointer
 * - output vector pointer (must be empty)
 */
void neunet_calculate_outputs(struct __neunet_t__ *, vector_t *, vector_t *);

/**
 * randomize weights of a neural network
 *
 * parameters:
 * - neural network struct pointer
 */
void neunet_randomize_weights(struct __neunet_t__ *);

#endif // __NEUNET_H__
