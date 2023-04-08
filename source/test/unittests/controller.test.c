#include "../../laplace/controller.h"

#define KIT_TEST_FILE controller
#include <kit_test/test.h>

static int test_state_ = -1;

STATIC_CORO(laplace_impact_list_t, test_exe_state_0_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  if (test_state_ == -1)
    test_state_ = 0;
}
CORO_END

STATIC_CORO(laplace_impact_list_t, test_exe_state_1_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  if (test_state_ == 0)
    test_state_ = 1;
}
CORO_END

static action_t const test_state_0_ = ACTION_UNSAFE(test_exe_state_0_,
                                                    1, HANDLE_NULL);

static action_t const test_state_1_ = ACTION_UNSAFE(test_exe_state_1_,
                                                    1, HANDLE_NULL);

TEST("controller schedule two events") {
  test_state_ = -1;

  read_write_t  state;
  thread_pool_t pool;

  memset(&state, 0, sizeof state);
  memset(&pool, 0, sizeof pool);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          KIT_OK);

  controller_t ctrl;
  REQUIRE(controller_init(&ctrl, 0, kit_alloc_default()) == KIT_OK);

  event_t ev_0 = { .time = 0, .action = test_state_0_ };
  event_t ev_1 = { .time = 1, .action = test_state_1_ };

  REQUIRE(controller_queue(&ctrl, ev_0) == KIT_OK);
  REQUIRE(controller_queue(&ctrl, ev_1) == KIT_OK);

  REQUIRE(test_state_ == -1);
  REQUIRE(schedule_and_join(&ctrl, &exe, 1) == KIT_OK);
  REQUIRE(test_state_ == 0);
  REQUIRE(schedule_and_join(&ctrl, &exe, 1) == KIT_OK);
  REQUIRE(test_state_ == 1);
  REQUIRE(schedule_and_join(&ctrl, &exe, 1) == KIT_OK);
  REQUIRE(test_state_ == 1);

  controller_destroy(&ctrl);
  execution_destroy(&exe);
}

