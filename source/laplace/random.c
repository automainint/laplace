#include "random.h"

#include <stdlib.h>
#include <time.h>

laplace_random_state_t laplace_random_seed() {
  srand((unsigned) time(NULL));
  laplace_random_state_t x;
  x.x = rand();
  return x;
}

void laplace_random_init(laplace_random_state_t *state,
                         int64_t                 seed) {
  state->x = seed;
}

int64_t laplace_random(laplace_random_state_t *state, int64_t min,
                       int64_t max) {
  int64_t x = rand_r((unsigned *) &state->x);
  return min + x % (max - min + 1);
}
