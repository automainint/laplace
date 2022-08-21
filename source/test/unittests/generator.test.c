#include "../../laplace/generator.h"
#include <string.h>

#define KIT_TEST_FILE buffer
#include <kit_test/test.h>

CORO(impact_list_t, test_impact_gen_, kit_allocator_t alloc;
     read_only_t access; handle_t self;) {
  impact_list_t list;

  DA_INIT(list, 1, af alloc);
  list.values[0].type = IMPACT_NOOP;
  AF_YIELD(list);

  DA_INIT(list, 2, af alloc);
  list.values[0].type = IMPACT_NOOP;
  list.values[1].type = IMPACT_NOOP;
  AF_RETURN(list);
}
CORO_END

static int alloc_count, free_count;

static void *allocate(void *_, size_t size) {
  alloc_count++;
  return kit_alloc_default().allocate(_, size);
}

static void deallocate(void *_, void *p) {
  free_count++;
  kit_alloc_default().deallocate(_, p);
}

TEST("generator example") {
  alloc_count = 0;
  free_count  = 0;

  kit_allocator_t alloc = { .state      = NULL,
                            .allocate   = allocate,
                            .deallocate = deallocate };

  action_t action = ACTION(test_impact_gen_, 1, handle_null);

  read_only_t access;
  memset(&access, 0, sizeof access);

  generator_t gen;
  REQUIRE(generator_init(&gen, action, access, alloc) == STATUS_OK);
  REQUIRE(generator_status(&gen) == GENERATOR_RUNNING);

  impact_list_t foo = generator_run(&gen);
  REQUIRE(foo.size == 1);
  DA_DESTROY(foo);

  impact_list_t bar = generator_run(&gen);
  REQUIRE(bar.size == 2);
  DA_DESTROY(bar);

  REQUIRE(generator_status(&gen) == GENERATOR_FINISHED);
  generator_destroy(gen);

  REQUIRE(alloc_count == free_count);
}
