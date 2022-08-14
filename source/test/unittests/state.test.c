#include "../../laplace/state.h"
#include <string.h>

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
  state_init(&access, alloc);

  REQUIRE(alloc_count > 0);
  REQUIRE(free_count == 0);

  access.acquire(access.state);
  access.release(access.state);

  REQUIRE(alloc_count > 0);
  REQUIRE(alloc_count == free_count);
}