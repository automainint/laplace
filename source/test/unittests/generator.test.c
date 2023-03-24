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

  action_t action = ACTION(0, test_impact_gen_, 1, handle_null);

  read_only_t access;
  memset(&access, 0, sizeof access);

  generator_t gen;
  REQUIRE(generator_init(&gen, action, access, alloc) == KIT_OK);
  REQUIRE(generator_status(&gen) == GENERATOR_RUNNING);

  impact_list_t foo = generator_run(&gen);
  REQUIRE(foo.size == 1);
  DA_DESTROY(foo);

  impact_list_t bar = generator_run(&gen);
  REQUIRE(bar.size == 2);
  DA_DESTROY(bar);

  REQUIRE(generator_status(&gen) == GENERATOR_FINISHED);

  REQUIRE(alloc_count == free_count);
}

#ifdef LAPLACE_ENABLE_STATIC_DISPATCH
enum { TEST_ACTION_42 = 42 };

CORO(impact_list_t, test_action_42_, kit_allocator_t alloc;
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

int test_dispatch_fallback_ = 0;

void laplace_action_dispatch(ptrdiff_t const          action_id,
                             laplace_promise_t *const promise) {
  switch (action_id) {
    case TEST_ACTION_42:
      /*  Execute the coroutine.
       */
      test_action_42_(promise, af_request_execute);
      break;

    default:
      /*  Fallback to dynamic dispatch.
       */
      test_dispatch_fallback_ = 1;
      AF_EXECUTE(promise);
      break;
  }
}

TEST("generator static dispatch example") {
  alloc_count             = 0;
  free_count              = 0;
  test_dispatch_fallback_ = 0;

  kit_allocator_t alloc = { .state      = NULL,
                            .allocate   = allocate,
                            .deallocate = deallocate };

  action_t action = ACTION(TEST_ACTION_42, test_action_42_, 1,
                           handle_null);

  read_only_t access;
  memset(&access, 0, sizeof access);

  generator_t gen;
  REQUIRE(generator_init(&gen, action, access, alloc) == KIT_OK);
  REQUIRE(generator_status(&gen) == GENERATOR_RUNNING);

  impact_list_t foo = generator_run(&gen);
  REQUIRE(foo.size == 1);
  DA_DESTROY(foo);

  impact_list_t bar = generator_run(&gen);
  REQUIRE(bar.size == 2);
  DA_DESTROY(bar);

  REQUIRE(generator_status(&gen) == GENERATOR_FINISHED);

  REQUIRE(test_dispatch_fallback_ == 0);
  REQUIRE(alloc_count == free_count);
}
#endif
