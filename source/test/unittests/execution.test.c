#include "../../laplace/execution.h"
#include "../../laplace/state.h"

#define KIT_TEST_FILE execution
#include <kit_test/test.h>

typedef struct {
  DA(threads, thrd_t);
} pool_state_t_;

static void pool_join_(void *state) {
  pool_state_t_ *p = (pool_state_t_ *) state;
  for (ptrdiff_t i = 0; i < p->threads.size; i++)
    (void) thrd_join(p->threads.values[i], NULL);
}

static laplace_status_t pool_resize_(void *state, ptrdiff_t size,
                                     pool_routine_fn routine,
                                     execution_t    *execution) {
  if (size < 0)
    return ERROR_INVALID_SIZE;
  pool_state_t_  *p = (pool_state_t_ *) state;
  ptrdiff_t const n = size < p->threads.size ? 0 : p->threads.size;
  DA_RESIZE(p->threads, size);
  if (p->threads.size != size)
    return ERROR_BAD_ALLOC;
  for (ptrdiff_t i = n; i < size; i++)
    if (thrd_create(p->threads.values + i, routine, execution) !=
        thrd_success)
      return ERROR_BAD_THREAD_CREATE;
  return STATUS_OK;
}

TEST("execution init and destroy") {
  read_write_t          state;
  laplace_thread_pool_t pool;

  memset(&state, 0, sizeof state);
  memset(&pool, 0, sizeof pool);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          STATUS_OK);
  REQUIRE(exe.status == STATUS_OK);
  execution_destroy(&exe);
}

TEST("execution get thread count") {
  read_write_t          state;
  laplace_thread_pool_t pool;

  memset(&state, 0, sizeof state);
  memset(&pool, 0, sizeof pool);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          STATUS_OK);
  REQUIRE(exe.thread_count == 0);
  execution_destroy(&exe);
}

TEST("execution set thread count") {
  read_write_t state;
  memset(&state, 0, sizeof state);

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, kit_alloc_default());
  laplace_thread_pool_t pool = { .state  = &pool_,
                                 .resize = pool_resize_,
                                 .join   = pool_join_ };

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          STATUS_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == STATUS_OK);
  REQUIRE(exe.thread_count == 4);
  execution_destroy(&exe);
  DA_DESTROY(pool_.threads);
}

TEST("execution set thread count to zero") {
  read_write_t state;
  memset(&state, 0, sizeof state);

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, kit_alloc_default());
  laplace_thread_pool_t pool = { .state  = &pool_,
                                 .resize = pool_resize_,
                                 .join   = pool_join_ };

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          STATUS_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == STATUS_OK);
  REQUIRE(execution_set_thread_count(&exe, 0) == STATUS_OK);
  REQUIRE(exe.thread_count == 0);
  execution_destroy(&exe);
  DA_DESTROY(pool_.threads);
}

static int action_status_ = 0;

CORO(laplace_impact_list_t, test_exe_noop_, kit_allocator_t alloc;
     read_only_t access; handle_t self;) {
  action_status_++;
}
CORO_END

CORO(laplace_impact_list_t, test_exe_null_self_,
     kit_allocator_t alloc;
     read_only_t access; handle_t self;) {
  action_status_ = (int) af access.integers_size(af access.state,
                                                 af self);
}
CORO_END

static action_t const test_noop_ = ACTION_UNSAFE(test_exe_noop_, 1,
                                                 HANDLE_NULL);
static action_t const test_null_self_ = ACTION_UNSAFE(
    test_exe_null_self_, 1, HANDLE_NULL);

TEST("seq execution queue action") {
  action_status_ = 0;

  read_write_t          state;
  laplace_thread_pool_t pool;

  memset(&state, 0, sizeof state);
  memset(&pool, 0, sizeof pool);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          STATUS_OK);
  REQUIRE(execution_queue(&exe, test_noop_) == STATUS_OK);
  execution_destroy(&exe);

  REQUIRE(!action_status_);
}

TEST("seq execution queue action and schedule") {
  action_status_ = 0;

  read_write_t          state;
  laplace_thread_pool_t pool;

  memset(&state, 0, sizeof state);
  memset(&pool, 0, sizeof pool);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          STATUS_OK);
  REQUIRE(execution_queue(&exe, test_noop_) == STATUS_OK);
  REQUIRE(execution_schedule_and_join(&exe, 1) == STATUS_OK);
  execution_destroy(&exe);

  REQUIRE(action_status_);
}

TEST("seq execution queue action and schedule zero") {
  action_status_ = 0;

  read_write_t          state;
  laplace_thread_pool_t pool;

  memset(&state, 0, sizeof state);
  memset(&pool, 0, sizeof pool);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          STATUS_OK);
  REQUIRE(execution_queue(&exe, test_noop_) == STATUS_OK);
  REQUIRE(execution_schedule_and_join(&exe, 0) == STATUS_OK);
  execution_destroy(&exe);

  REQUIRE(!action_status_);
}

TEST("seq execution queue action and access null self") {
  action_status_ = -1;

  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == STATUS_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == STATUS_OK);

  REQUIRE(execution_queue(&exe, test_null_self_) == STATUS_OK);
  REQUIRE(execution_schedule_and_join(&exe, 1) == STATUS_OK);
  execution_destroy(&exe);

  REQUIRE(action_status_ == 0);
}

TEST("seq execution read only") {
  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == STATUS_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == STATUS_OK);
  read_only_t read_only = execution_read_only(&exe);
  read_only.acquire(read_only.state);
  REQUIRE(read_only.get_integer(read_only.state, handle_null, 0,
                                -1) == -1);
  read_only.release(read_only.state);
  execution_destroy(&exe);
}

laplace_integer_t test_state_get_integer_(void *_, handle_t h,
                                          ptrdiff_t         index,
                                          laplace_integer_t invalid) {
  return 42;
}

TEST("seq execution custom state") {
  read_write_t state;
  memset(&state, 0, sizeof state);
  state.get_integer = test_state_get_integer_;

  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == STATUS_OK);
  read_only_t read_only = execution_read_only(&exe);
  REQUIRE(read_only.get_integer(read_only.state, handle_null, 0,
                                -1) == 42);
  execution_destroy(&exe);
}
