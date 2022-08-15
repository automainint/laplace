#include "../../laplace/generator.h"
#include <string.h>

#define KIT_TEST_FILE buffer
#include <kit_test/test.h>

CORO(impact_list_t, test_impact_gen, kit_allocator_t alloc;
     read_only_t access; int arg;) {
  impact_list_t list;

  DA_INIT(list, 1, af alloc);
  list.values[0].type = IMPACT_NOOP;
  AF_YIELD(list);

  DA_INIT(list, af arg, af alloc);
  for (int i = 0; i < af arg; i++) list.values[i].type = IMPACT_NOOP;
  AF_RETURN(list);
}
CORO_END

static laplace_status_t test_starter(void *coro, void *data,
                                     kit_allocator_t alloc,
                                     read_only_t     access) {
  AF_INIT(*(AF_TYPE(test_impact_gen) *) coro, test_impact_gen,
          .alloc = alloc, .access = access, .arg = *(int *) data);
  return STATUS_OK;
}

int alloc_count, free_count;

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

  int *data = (int *) alloc.allocate(alloc.state, sizeof(int));
  *data     = 2;

  action_t action = { .size    = sizeof(AF_TYPE(test_impact_gen)),
                      .data    = data,
                      .alloc   = alloc,
                      .starter = test_starter,
                      .tick_duration = 1 };

  laplace_read_only_t access;
  memset(&access, 0, sizeof access);

  laplace_generator_t gen;
  REQUIRE(laplace_generator_init(&gen, action, access) == STATUS_OK);
  REQUIRE(laplace_generator_status(gen) == GENERATOR_RUNNING);

  impact_list_t foo = generator_run(gen);
  REQUIRE(foo.size == 1);
  DA_DESTROY(foo);

  impact_list_t bar = generator_run(gen);
  REQUIRE(bar.size == 2);
  DA_DESTROY(bar);

  REQUIRE(laplace_generator_status(gen) == GENERATOR_FINISHED);
  laplace_generator_destroy(gen);

  REQUIRE(alloc_count == free_count);
}
