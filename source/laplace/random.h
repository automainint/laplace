#ifndef LAPLACE_RANDOM_H
#define LAPLACE_RANDOM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint64_t x;
} laplace_random_state_t;

laplace_random_state_t laplace_random_seed();

void laplace_random_init(laplace_random_state_t *state, int64_t seed);

int64_t laplace_random(laplace_random_state_t *state, int64_t min,
                       int64_t max);

#ifdef __cplusplus
}
#endif

#endif
