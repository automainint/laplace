#ifndef LAPLACE_RNG_H
#define LAPLACE_RNG_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  LAPLACE_RNG_N = 312,
};

typedef struct {
  uint64_t mt[LAPLACE_RNG_N];
  uint64_t index;
} laplace_rng_state_t;

void laplace_rng_init(laplace_rng_state_t *state, uint64_t seed);

int64_t laplace_rng(laplace_rng_state_t *state, int64_t min,
                    int64_t max);

uint64_t laplace_rng_seed();

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define RNG_N LAPLACE_RNG_N
#  define RNG_M LAPLACE_RNG_M
#  define RNG_MATRIX_A LAPLACE_RNG_MATRIX_A
#  define RNG_UPPER_MASK LAPLACE_RNG_UPPER_MASK
#  define RNG_LOWER_MASK LAPLACE_RNG_LOWER_MASK

#  define rng_state_t laplace_rng_state_t
#  define rng_seed laplace_rng_seed
#  define rng_init laplace_rng_init
#  define rng laplace_rng
#endif

#ifdef __cplusplus
}
#endif

#endif
