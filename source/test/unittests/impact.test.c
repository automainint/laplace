#include "../../laplace/impact.h"

#define KIT_TEST_FILE impact
#include <kit_test/test.h>

TEST("impact noop") {
  impact_t i = NOOP();
  REQUIRE(impact_mode_of(&i) == IMPACT_ASYNC);
}

TEST("impact tick continue") {
  impact_t i = TICK_CONTINUE();
  REQUIRE(impact_mode_of(&i) == IMPACT_CONTROL);
}

TEST("impact integer reserve") {
  impact_t i = INTEGER_RESERVE(0);
  REQUIRE(impact_mode_of(&i) == IMPACT_SYNC);
}

TEST("impact integer allocate into") {
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = INTEGER_ALLOCATE_INTO(h, 0);
  REQUIRE(impact_mode_of(&i) == IMPACT_SYNC);
}

TEST("impact integer allocate") {
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = INTEGER_ALLOCATE(0, h, 0);
  REQUIRE(impact_mode_of(&i) == IMPACT_SYNC);
}

TEST("impact integer deallocate") {
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = INTEGER_DEALLOCATE(h);
  REQUIRE(impact_mode_of(&i) == IMPACT_SYNC);
}

TEST("impact integer set") {
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = INTEGER_SET(h, 0, 0);
  REQUIRE(impact_mode_of(&i) == IMPACT_ASYNC);
}

TEST("impact integer add") {
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = INTEGER_ADD(h, 0, 0);
  REQUIRE(impact_mode_of(&i) == IMPACT_ASYNC);
}

TEST("impact integer write values") {
  handle_t h = { .id = 0, .generation = -1 };
  DA_CREATE(v, laplace_integer_t, 0);
  impact_t i = INTEGER_WRITE_VALUES(h, 0, v);
  REQUIRE(impact_mode_of(&i) == IMPACT_ASYNC);
}

TEST("impact integer write deltas") {
  handle_t h = { .id = 0, .generation = -1 };
  DA_CREATE(v, laplace_integer_t, 0);
  impact_t i = INTEGER_WRITE_DELTAS(h, 0, v);
  REQUIRE(impact_mode_of(&i) == IMPACT_ASYNC);
}

TEST("impact byte reserve") {
  impact_t i = BYTE_RESERVE(0);
  REQUIRE(impact_mode_of(&i) == IMPACT_SYNC);
}

TEST("impact byte allocate into") {
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = BYTE_ALLOCATE_INTO(h, 0);
  REQUIRE(impact_mode_of(&i) == IMPACT_SYNC);
}

TEST("impact byte allocate") {
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = BYTE_ALLOCATE(0, h, 0);
  REQUIRE(impact_mode_of(&i) == IMPACT_SYNC);
}

TEST("impact byte deallocate") {
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = BYTE_DEALLOCATE(h);
  REQUIRE(impact_mode_of(&i) == IMPACT_SYNC);
}

TEST("impact byte set") {
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = BYTE_SET(h, 0, 0);
  REQUIRE(impact_mode_of(&i) == IMPACT_ASYNC);
}

TEST("impact byte add") {
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = BYTE_ADD(h, 0, 0);
  REQUIRE(impact_mode_of(&i) == IMPACT_ASYNC);
}

TEST("impact byte write values") {
  handle_t h = { .id = 0, .generation = -1 };
  DA_CREATE(v, laplace_byte_t, 0);
  impact_t i = BYTE_WRITE_VALUES(h, 0, v);
  REQUIRE(impact_mode_of(&i) == IMPACT_ASYNC);
}

TEST("impact byte write deltas") {
  handle_t h = { .id = 0, .generation = -1 };
  DA_CREATE(v, laplace_byte_t, 0);
  impact_t i = BYTE_WRITE_DELTAS(h, 0, v);
  REQUIRE(impact_mode_of(&i) == IMPACT_ASYNC);
}

TEST("impact integer seed") {
  impact_t i = INTEGER_SEED(0);
  REQUIRE(impact_mode_of(&i) == IMPACT_SYNC);
}

TEST("impact integer random") {
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = INTEGER_RANDOM(0, 0, h, 0, 0);
  REQUIRE(impact_mode_of(&i) == IMPACT_SYNC);
}

TEST("impact byte random") {
  handle_t h = { .id = 0, .generation = -1 };
  impact_t i = BYTE_RANDOM(0, 0, h, 0, 0);
  REQUIRE(impact_mode_of(&i) == IMPACT_SYNC);
}

TEST("impact queue action") {
  action_t a;
  memset(&a, 0, sizeof a);
  impact_t i = QUEUE_ACTION(a);
  REQUIRE(impact_mode_of(&i) == IMPACT_SYNC);
}

static int alloc_count = 0;
static int free_count  = 0;

static void *allocate(int request, void *_, ptrdiff_t size,
                      ptrdiff_t previous_size, void *pointer) {
  switch (request) {
    case KIT_ALLOCATE: alloc_count++; break;
    case KIT_DEALLOCATE: free_count++; break;
    default:;
  }

  return kit_alloc_dispatch(kit_alloc_default(), request, size,
                            previous_size, pointer);
}

TEST("impact list allocations") {
  alloc_count = 0;
  free_count  = 0;

  kit_allocator_t alloc = { .state = NULL, .allocate = allocate };

  handle_t h = { .id = 0, .generation = -1 };

  laplace_integers_t iv;
  laplace_integers_t id;
  laplace_bytes_t    bv;
  laplace_bytes_t    bd;

  DA_INIT(iv, 0, alloc);
  DA_INIT(id, 0, alloc);
  DA_INIT(bv, 0, alloc);
  DA_INIT(bd, 0, alloc);

  impact_t a = INTEGER_WRITE_VALUES(h, 0, iv);
  impact_t b = INTEGER_WRITE_DELTAS(h, 0, id);
  impact_t c = BYTE_WRITE_VALUES(h, 0, bv);
  impact_t d = BYTE_WRITE_DELTAS(h, 0, bd);
  impact_t e = NOOP();

  impact_list_t list;
  DA_INIT(list, 0, alloc);

  DA_INSERT(list, list.size, a);
  DA_INSERT(list, list.size, b);
  DA_INSERT(list, list.size, c);
  DA_INSERT(list, list.size, d);
  DA_INSERT(list, list.size, e);

  laplace_impact_list_destroy(&list);

  REQUIRE(alloc_count == free_count);
}
