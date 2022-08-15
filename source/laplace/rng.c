#include "rng.h"

#include <time.h>

void laplace_rng_init(laplace_rng_state_t *state, uint64_t seed) {
  state->mt[0] = seed;
  for (state->index = 1; state->index < LAPLACE_RNG_N; state->index++)
    state->mt[state->index] = (6364136223846793005ull *
                                   (state->mt[state->index - 1] ^
                                    (state->mt[state->index - 1] >>
                                     62u)) +
                               state->index);
}

static uint64_t rng_u64(laplace_rng_state_t *state) {
  static uint64_t const mag01[2] = { 0ull, 0xB5026F5AA96619E9ull };

  int      i;
  uint64_t x;

  if (state->index >= LAPLACE_RNG_N) {
    for (i = 0; i < LAPLACE_RNG_N - 156; i++) {
      x = (state->mt[i] & 0xFFFFFFFF80000000ull) |
          (state->mt[i + 1] & 0x7FFFFFFFull);
      state->mt[i] = state->mt[i + 156] ^ (x >> 1u) ^
                     mag01[(int) (x & 1ull)];
    }

    for (; i < LAPLACE_RNG_N - 1; i++) {
      x = (state->mt[i] & 0xFFFFFFFF80000000ull) |
          (state->mt[i + 1] & 0x7FFFFFFFull);
      state->mt[i] = state->mt[i + (156 - LAPLACE_RNG_N)] ^
                     (x >> 1u) ^ mag01[(int) (x & 1ull)];
    }

    x = (state->mt[LAPLACE_RNG_N - 1] & 0xFFFFFFFF80000000ull) |
        (state->mt[0] & 0x7FFFFFFFull);
    state->mt[LAPLACE_RNG_N - 1] = state->mt[156 - 1] ^ (x >> 1u) ^
                                   mag01[(int) (x & 1ull)];

    state->index = 0;
  }

  x = state->mt[state->index++];

  x ^= (x >> 29u) & 0x5555555555555555ull;
  x ^= (x << 17u) & 0x71D67FFFEDA60000ull;
  x ^= (x << 37u) & 0xFFF7EEE000000000ull;
  x ^= (x >> 43u);

  return x;
}

int64_t laplace_rng(laplace_rng_state_t *state, int64_t min,
                    int64_t max) {
  uint64_t x = rng_u64(state);
  return min + (int64_t) (x % (uint64_t) (max - min + 1));
}

uint64_t laplace_rng_seed() {
  struct timespec t;
  timespec_get(&t, TIME_UTC);

  laplace_rng_state_t s;
  laplace_rng_init(&s, (uint64_t) t.tv_nsec);

  return rng_u64(&s);
}
