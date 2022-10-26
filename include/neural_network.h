#ifndef __NEURAL_NETWORK_H__
#define __NEURAL_NETWORK_H__

#include <libcollections/vector.h>
#include <stdlib.h>

struct __neural_network_t__ {
	vector_t layer_vec;
};

struct __neural_network_layer_t__ {
	uint input_len;
	uint output_len;
	vector_t weight_vec;
	vector_t bias_vec;
};

static const double NEURAL_NETWORK_WEIGHT_AMPLITUDE = 0.1;

typedef struct __neural_network_layer_t__ neural_network_layer_t;
typedef struct __neural_network_t__ neural_network_t;

void neural_network_init(neural_network_t *, vector_t *);
void neural_network_calc_outputs(neural_network_t *, vector_t *, vector_t *);
void neural_network_drop(neural_network_t *);
void neural_network_randomize_weights(neural_network_t *);

#endif // __NEURAL_NETWORK_H__
