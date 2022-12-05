#include "../../laplace/state.h"
#include "../../laplace/impact.h"
#include <kit/mersenne_twister_64.h>

#define KIT_TEST_FILE state
#include <kit_test/test.h>

static int alloc_count = 0;
static int free_count  = 0;

static void *allocate(void *_, size_t size) {
  alloc_count++;
  return kit_alloc_default().allocate(_, size);
}

static void deallocate(void *_, void *p) {
  free_count++;
  kit_alloc_default().deallocate(_, p);
}

TEST("state acquire and release") {
  kit_allocator_t alloc = { .state      = NULL,
                            .allocate   = allocate,
                            .deallocate = deallocate };

  alloc_count = 0;
  free_count  = 0;

  read_write_t access;
  state_init(&access, mt64_seed(), alloc);

  REQUIRE(alloc_count > 0);
  REQUIRE(free_count == 0);

  access.acquire(access.state);
  access.release(access.state);

  REQUIRE(alloc_count > 0);
  REQUIRE(alloc_count == free_count);
}

TEST("state apply noop") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  impact_t i = NOOP();
  REQUIRE(a.apply(a.state, &i) == KIT_OK);
  a.release(a.state);
}

TEST("state apply may fail") {
  impact_t     i = TICK_CONTINUE();
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  REQUIRE(a.apply(a.state, &i) == ERROR_WRONG_IMPACT);
  a.release(a.state);
}

TEST("state get int may fail") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t h = { .id = 0, .generation = 0 };
  REQUIRE(a.get_integer(a.state, h, 0, -1) == -1);
  a.release(a.state);
}

TEST("state apply allocate into int and get") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t h = { .id = 42, .generation = -1 };
  impact_t i = INTEGER_ALLOCATE_INTO(h, 1);
  REQUIRE(a.apply(a.state, &i) == KIT_OK);
  h.generation++;
  REQUIRE(a.get_integer(a.state, h, 0, -1) == 0);
  a.release(a.state);
}

TEST("state apply allocate int and get") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t ret = { .id = 0, .generation = -1 };
  impact_t i   = INTEGER_ALLOCATE_INTO(ret, 2);
  REQUIRE(a.apply(a.state, &i) == KIT_OK);
  ret.generation++;
  impact_t j = INTEGER_ALLOCATE(1, ret, 0);
  REQUIRE(a.apply(a.state, &j) == KIT_OK);
  a.adjust_loop(a.state);
  handle_t h = { .id         = a.get_integer(a.state, ret, 0, -1),
                 .generation = a.get_integer(a.state, ret, 1, -1) };
  REQUIRE(h.id != -1);
  REQUIRE(h.generation != -1);
  REQUIRE(a.get_integer(a.state, h, 0, -1) == 0);
  a.release(a.state);
}

TEST("state apply reserve int and allocate into") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  impact_t i = INTEGER_RESERVE(10);
  REQUIRE(a.apply(a.state, &i) == KIT_OK);
  handle_t h = { .id = 0, .generation = -1 };
  impact_t j = INTEGER_ALLOCATE_INTO(h, 1);
  h.generation++;
  REQUIRE(a.apply(a.state, &j) == KIT_OK);
  REQUIRE(a.get_integer(a.state, h, 0, -1) == 0);
  a.release(a.state);
}

TEST("state apply reserve int and allocate") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  ptrdiff_t reserved = 10;
  impact_t  i        = INTEGER_RESERVE(reserved);
  REQUIRE(a.apply(a.state, &i) == KIT_OK);
  handle_t ret = { .id = 0, .generation = -1 };
  impact_t j   = INTEGER_ALLOCATE_INTO(ret, 4);
  REQUIRE(a.apply(a.state, &j) == KIT_OK);
  ret.generation++;
  impact_t k = INTEGER_ALLOCATE(1, ret, 0);
  REQUIRE(a.apply(a.state, &k) == KIT_OK);
  impact_t l = INTEGER_ALLOCATE(1, ret, 2);
  REQUIRE(a.apply(a.state, &l) == KIT_OK);
  a.adjust_loop(a.state);
  REQUIRE(a.get_integer(a.state, ret, 0, -1) == reserved);
  REQUIRE(a.get_integer(a.state, ret, 1, -1) == 0);
  REQUIRE(a.get_integer(a.state, ret, 2, -1) == reserved + 1);
  REQUIRE(a.get_integer(a.state, ret, 3, -1) == 0);
  a.release(a.state);
}

TEST("state apply allocate int, set and get") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t ret = { .id = 0, .generation = -1 };
  impact_t i   = INTEGER_ALLOCATE_INTO(ret, 2);
  REQUIRE(a.apply(a.state, &i) == KIT_OK);
  ret.generation++;
  impact_t j = INTEGER_ALLOCATE(1, ret, 0);
  REQUIRE(a.apply(a.state, &j) == KIT_OK);
  a.adjust_loop(a.state);
  a.adjust_done(a.state);
  handle_t h = { .id         = a.get_integer(a.state, ret, 0, -1),
                 .generation = a.get_integer(a.state, ret, 1, -1) };
  REQUIRE(h.id != -1);
  REQUIRE(h.generation == 0);
  impact_t k = INTEGER_SET(h, 0, 42);
  REQUIRE(a.apply(a.state, &k) == KIT_OK);
  a.adjust_loop(a.state);
  REQUIRE(a.get_integer(a.state, h, 0, -1) == 42);
  a.release(a.state);
}

TEST("state deallocate int") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = INTEGER_ALLOCATE_INTO(h, 1);
  REQUIRE(a.apply(a.state, &i) == KIT_OK);
  h.generation++;
  impact_t j = INTEGER_DEALLOCATE(h);
  REQUIRE(a.apply(a.state, &j) == KIT_OK);
  a.release(a.state);
}

TEST("state apply add int") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t h0  = { .id = 0, .generation = -1 };
  handle_t h   = { .id = 0, .generation = 0 };
  impact_t i[] = { INTEGER_ALLOCATE_INTO(h0, 1),
                   INTEGER_ADD(h, 0, 18), INTEGER_ADD(h, 0, 24) };
  REQUIRE(a.apply(a.state, i) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 1) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 2) == KIT_OK);
  a.adjust_loop(a.state);
  REQUIRE(a.get_integer(a.state, h, 0, -1) == 42);
  a.release(a.state);
}

TEST("state get byte may fail") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t h = { .id = 0, .generation = 0 };
  REQUIRE(a.get_byte(a.state, h, 0, -1) == -1);
  a.release(a.state);
}

TEST("state apply allocate into byte and get") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t h   = { .id = 42, .generation = -1 };
  impact_t i[] = { BYTE_ALLOCATE_INTO(h, 1) };
  REQUIRE(a.apply(a.state, i) == KIT_OK);
  h.generation++;
  REQUIRE(a.get_byte(a.state, h, 0, -1) == 0);
  a.release(a.state);
}

TEST("state apply allocate byte and get") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t ret0 = { .id = 0, .generation = -1 };
  handle_t ret  = { .id = 0, .generation = 0 };
  impact_t i[]  = { INTEGER_ALLOCATE_INTO(ret0, 2),
                   BYTE_ALLOCATE(1, ret, 0) };
  REQUIRE(a.apply(a.state, i) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 1) == KIT_OK);
  a.adjust_loop(a.state);
  handle_t h = { .id         = a.get_integer(a.state, ret, 0, -1),
                 .generation = a.get_integer(a.state, ret, 1, -1) };
  REQUIRE(h.id != -1);
  REQUIRE(h.generation == 0);
  REQUIRE(a.get_byte(a.state, h, 0, -1) == 0);
  a.release(a.state);
}

TEST("state apply allocate byte, set and get") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t ret0 = { .id = 0, .generation = -1 };
  handle_t ret  = { .id = 0, .generation = 0 };
  handle_t h    = { .id = -1, .generation = -1 };
  impact_t i[]  = { INTEGER_ALLOCATE_INTO(ret0, 2),
                   BYTE_ALLOCATE(1, ret, 0), BYTE_SET(h, 0, 42) };
  REQUIRE(a.apply(a.state, i) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 1) == KIT_OK);
  a.adjust_loop(a.state);
  a.adjust_done(a.state);
  h.id         = a.get_integer(a.state, ret, 0, -1);
  h.generation = a.get_integer(a.state, ret, 1, -1);
  REQUIRE(h.id != -1);
  REQUIRE(h.generation == 0);
  i[2].byte_set.handle = h;
  REQUIRE(a.apply(a.state, i + 2) == KIT_OK);
  a.adjust_loop(a.state);
  REQUIRE(a.get_byte(a.state, h, 0, -1) == 42);
  a.release(a.state);
}

TEST("state apply reserve byte and allocate into") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t h0  = { .id = 0, .generation = -1 };
  handle_t h   = { .id = 0, .generation = 0 };
  impact_t i[] = { BYTE_RESERVE(10), BYTE_ALLOCATE_INTO(h0, 1) };
  REQUIRE(a.apply(a.state, i) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 1) == KIT_OK);
  REQUIRE(a.get_byte(a.state, h, 0, -1) == 0);
  a.release(a.state);
}

TEST("state apply reserve byte and allocate") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t  ret0     = { .id = 0, .generation = -1 };
  handle_t  ret      = { .id = 0, .generation = 0 };
  ptrdiff_t reserved = 10;
  impact_t  i[]      = { INTEGER_ALLOCATE_INTO(ret0, 4),
                   BYTE_RESERVE(reserved), BYTE_ALLOCATE(1, ret, 0),
                   BYTE_ALLOCATE(1, ret, 2) };
  REQUIRE(a.apply(a.state, i) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 1) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 2) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 3) == KIT_OK);
  a.adjust_loop(a.state);
  REQUIRE(a.get_integer(a.state, ret, 0, -1) == reserved);
  REQUIRE(a.get_integer(a.state, ret, 1, -1) == 0);
  REQUIRE(a.get_integer(a.state, ret, 2, -1) == reserved + 1);
  REQUIRE(a.get_integer(a.state, ret, 3, -1) == 0);
  a.release(a.state);
}

TEST("state deallocate byte") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t h0  = { .id = 0, .generation = -1 };
  handle_t h   = { .id = 0, .generation = 0 };
  impact_t i[] = { BYTE_ALLOCATE_INTO(h0, 1), BYTE_DEALLOCATE(h) };
  REQUIRE(a.apply(a.state, i) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 1) == KIT_OK);
  a.release(a.state);
}

TEST("state apply add byte") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t h0  = { .id = 0, .generation = -1 };
  handle_t h   = { .id = 0, .generation = 0 };
  impact_t i[] = { BYTE_ALLOCATE_INTO(h0, 1), BYTE_ADD(h, 0, 18),
                   BYTE_ADD(h, 0, 24) };
  REQUIRE(a.apply(a.state, i) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 1) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 2) == KIT_OK);
  a.adjust_loop(a.state);
  REQUIRE(a.get_byte(a.state, h, 0, -1) == 42);
  a.release(a.state);
}

TEST("state apply random") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t  h0   = { .id = 0, .generation = -1 };
  handle_t  h    = { .id = 0, .generation = 0 };
  ptrdiff_t size = 1000;
  impact_t  i[]  = { INTEGER_ALLOCATE_INTO(h0, size),
                   INTEGER_RANDOM(1, 100, h, 0, size) };
  REQUIRE(a.apply(a.state, i) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 1) == KIT_OK);
  a.adjust_loop(a.state);
  int ok = 1;
  for (ptrdiff_t k = 0; k < size; k++)
    ok = ok && ((a.get_integer(a.state, h, k, -1) >= 1 &&
                 a.get_integer(a.state, h, k, -1) <= 100));
  REQUIRE(ok);
  a.release(a.state);
}

TEST("state apply seed") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  impact_t i[] = { INTEGER_SEED(42) };
  REQUIRE(a.apply(a.state, i) == KIT_OK);
  a.release(a.state);
}

TEST("state random with equal seed") {
  read_write_t foo, bar;
  state_init(&foo, mt64_seed(), kit_alloc_default());
  state_init(&bar, mt64_seed(), kit_alloc_default());
  foo.acquire(foo.state);
  bar.acquire(bar.state);
  handle_t  h0   = { .id = 0, .generation = -1 };
  handle_t  h    = { .id = 0, .generation = 0 };
  ptrdiff_t size = 1000;
  impact_t  i[] = { INTEGER_SEED(42), INTEGER_ALLOCATE_INTO(h0, size),
                   INTEGER_RANDOM(1, 100, h, 0, size) };
  REQUIRE(foo.apply(foo.state, i) == KIT_OK);
  REQUIRE(bar.apply(bar.state, i) == KIT_OK);
  REQUIRE(foo.apply(foo.state, i + 1) == KIT_OK);
  REQUIRE(bar.apply(bar.state, i + 1) == KIT_OK);
  REQUIRE(foo.apply(foo.state, i + 2) == KIT_OK);
  REQUIRE(bar.apply(bar.state, i + 2) == KIT_OK);
  foo.adjust_loop(foo.state);
  bar.adjust_loop(bar.state);
  int ok = 1;
  for (ptrdiff_t k = 0; k < size; k++)
    ok = ok && foo.get_integer(foo.state, h, k, -1) ==
                   bar.get_integer(bar.state, h, k, -1);
  REQUIRE(ok);
  foo.release(foo.state);
  bar.release(bar.state);
}

TEST("state random with different seed") {
  read_write_t foo, bar;
  state_init(&foo, mt64_seed(), kit_alloc_default());
  state_init(&bar, mt64_seed(), kit_alloc_default());
  foo.acquire(foo.state);
  bar.acquire(bar.state);
  handle_t  h0   = { .id = 0, .generation = -1 };
  handle_t  h    = { .id = 0, .generation = 0 };
  ptrdiff_t size = 1000;
  impact_t  i[]  = { INTEGER_SEED(42), INTEGER_SEED(4242),
                   INTEGER_ALLOCATE_INTO(h0, size),
                   INTEGER_RANDOM(1, 100, h, 0, size) };
  REQUIRE(foo.apply(foo.state, i) == KIT_OK);
  REQUIRE(bar.apply(bar.state, i + 1) == KIT_OK);
  REQUIRE(foo.apply(foo.state, i + 2) == KIT_OK);
  REQUIRE(bar.apply(bar.state, i + 2) == KIT_OK);
  REQUIRE(foo.apply(foo.state, i + 3) == KIT_OK);
  REQUIRE(bar.apply(bar.state, i + 3) == KIT_OK);
  foo.adjust_loop(foo.state);
  bar.adjust_loop(bar.state);

  ptrdiff_t different_count = 0;
  for (ptrdiff_t k = 0; k < size; k++)
    if ((foo.get_integer(foo.state, h, k, -1) !=
         bar.get_integer(bar.state, h, k, -1)))
      different_count++;

  REQUIRE(different_count > size / 2);
  foo.release(foo.state);
  bar.release(bar.state);
}

TEST("state allocation determinism") {
  read_write_t a;
  state_init(&a, mt64_seed(), kit_alloc_default());
  a.acquire(a.state);
  handle_t h0 = { .id = 0, .generation = -1 };
  handle_t h  = { .id = 0, .generation = 0 };

  impact_t i[] = {
    INTEGER_ALLOCATE_INTO(h0, 10), INTEGER_ALLOCATE(10, h, 0),
    INTEGER_ALLOCATE(20, h, 2),    INTEGER_ALLOCATE(30, h, 4),
    INTEGER_ALLOCATE(40, h, 6),    INTEGER_ALLOCATE(50, h, 8)
  };

  impact_t  j[] = { INTEGER_DEALLOCATE(h) };
  handle_t *jh  = &j->integer_deallocate.handle;

  REQUIRE(a.apply(a.state, i) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 1) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 2) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 3) == KIT_OK);

  a.adjust_loop(a.state);
  a.adjust_done(a.state);

  REQUIRE(a.get_integer(a.state, h, 0, -1) == 1);
  REQUIRE(a.get_integer(a.state, h, 1, -1) == 0);
  REQUIRE(a.get_integer(a.state, h, 2, -1) == 2);
  REQUIRE(a.get_integer(a.state, h, 3, -1) == 0);
  REQUIRE(a.get_integer(a.state, h, 4, -1) == 3);
  REQUIRE(a.get_integer(a.state, h, 5, -1) == 0);

  jh->id         = a.get_integer(a.state, h, 0, -1);
  jh->generation = a.get_integer(a.state, h, 1, -1);
  REQUIRE(a.apply(a.state, j) == KIT_OK);

  REQUIRE(a.apply(a.state, i + 4) == KIT_OK);
  REQUIRE(a.apply(a.state, i + 5) == KIT_OK);

  a.adjust_loop(a.state);
  a.adjust_done(a.state);

  REQUIRE(a.get_integer(a.state, h, 6, -1) == 1);
  REQUIRE(a.get_integer(a.state, h, 7, -1) == 1);
  REQUIRE(a.get_integer(a.state, h, 8, -1) == 4);
  REQUIRE(a.get_integer(a.state, h, 9, -1) == 0);

  jh->id         = a.get_integer(a.state, h, 4, -1);
  jh->generation = a.get_integer(a.state, h, 5, -1);
  REQUIRE(a.apply(a.state, j) == KIT_OK);

  jh->id         = a.get_integer(a.state, h, 6, -1);
  jh->generation = a.get_integer(a.state, h, 7, -1);
  REQUIRE(a.apply(a.state, j) == KIT_OK);

  REQUIRE(a.apply(a.state, i + 1) == KIT_OK);

  a.adjust_loop(a.state);
  a.adjust_done(a.state);

  REQUIRE(a.get_integer(a.state, h, 0, -1) == 1);
  REQUIRE(a.get_integer(a.state, h, 1, -1) == 2);
  a.release(a.state);
}
