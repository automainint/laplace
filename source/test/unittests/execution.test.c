#include "../../laplace/execution.h"
#include "../../laplace/state.h"

#define KIT_TEST_FILE execution
#include <kit_test/test.h>

enum { default_tick = 10 };

typedef struct {
  DA(threads, thrd_t);
} pool_state_t_;

static void pool_release_(void *state) {
  pool_state_t_ *p = (pool_state_t_ *) state;
  DA_DESTROY(p->threads);
}

static void pool_join_(void *state) {
  pool_state_t_ *p = (pool_state_t_ *) state;
  for (ptrdiff_t i = 0; i < p->threads.size; i++)
    (void) thrd_join(p->threads.values[i], NULL);
  DA_RESIZE(p->threads, 0);
}

static kit_status_t pool_run_(void *state, ptrdiff_t count,
                              pool_routine_fn routine,
                              execution_t    *execution) {
  if (count <= 0)
    return ERROR_INVALID_SIZE;
  pool_state_t_  *p = (pool_state_t_ *) state;
  ptrdiff_t const n = p->threads.size;
  DA_RESIZE(p->threads, n + count);
  if (p->threads.size != n + count)
    return ERROR_BAD_ALLOC;
  for (ptrdiff_t i = 0; i < count; i++)
    if (thrd_create(p->threads.values + n + i, routine, execution) !=
        thrd_success)
      return ERROR_BAD_THREAD_CREATE;
  return KIT_OK;
}

TEST("execution init and destroy") {
  read_write_t          state;
  laplace_thread_pool_t pool;

  memset(&state, 0, sizeof state);
  memset(&pool, 0, sizeof pool);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          KIT_OK);
  REQUIRE(exe.status == KIT_OK);
  execution_destroy(&exe);
}

TEST("execution get thread count") {
  read_write_t          state;
  laplace_thread_pool_t pool;

  memset(&state, 0, sizeof state);
  memset(&pool, 0, sizeof pool);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          KIT_OK);
  REQUIRE(exe.thread_count == 0);
  execution_destroy(&exe);
}

TEST("execution set thread count") {
  read_write_t state;
  memset(&state, 0, sizeof state);

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, kit_alloc_default());
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == KIT_OK);
  REQUIRE(exe.thread_count == 4);
  execution_destroy(&exe);
}

TEST("execution set thread count to zero") {
  read_write_t state;
  memset(&state, 0, sizeof state);

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, kit_alloc_default());
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 0) == KIT_OK);
  REQUIRE(exe.thread_count == 0);
  execution_destroy(&exe);
}

static int action_status_ = 0;

STATIC_CORO(laplace_impact_list_t, test_exe_noop_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  action_status_++;
}
CORO_END

STATIC_CORO(laplace_impact_list_t, test_exe_null_self_,
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
          KIT_OK);
  REQUIRE(execution_queue(&exe, test_noop_) == KIT_OK);
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
          KIT_OK);
  REQUIRE(execution_queue(&exe, test_noop_) == KIT_OK);
  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);
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
          KIT_OK);
  REQUIRE(execution_queue(&exe, test_noop_) == KIT_OK);
  REQUIRE(execution_schedule_and_join(&exe, 0) == KIT_OK);
  execution_destroy(&exe);

  REQUIRE(!action_status_);
}

TEST("seq execution queue action and access null self") {
  action_status_ = -1;

  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);

  REQUIRE(execution_queue(&exe, test_null_self_) == KIT_OK);
  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);
  execution_destroy(&exe);

  REQUIRE(action_status_ == 0);
}

TEST("seq execution read only") {
  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  read_only_t read_only = execution_read_only(&exe);
  read_only.acquire(read_only.state);
  REQUIRE(read_only.get_integer(read_only.state, handle_null, 0,
                                -1) == -1);
  read_only.release(read_only.state);
  execution_destroy(&exe);
}

static laplace_integer_t test_state_get_integer_(
    void *_, handle_t h, ptrdiff_t index, laplace_integer_t invalid) {
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
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  read_only_t read_only = execution_read_only(&exe);
  REQUIRE(read_only.get_integer(read_only.state, handle_null, 0,
                                -1) == 42);
  execution_destroy(&exe);
}

STATIC_CORO(impact_list_t, test_exe_alloc_set_, kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  static handle_t const h0 = { .id = 0, .generation = -1 };
  static handle_t const h  = { .id = 0, .generation = 0 };

  DA_INIT(af return_value, 2, af alloc);
  impact_t i0[] = { INTEGER_ALLOCATE_INTO(h0, 1),
                    INTEGER_SET(h, 0, 42) };

  af return_value.values[0] = i0[0];
  af return_value.values[1] = i0[1];
  AF_YIELD_VOID;

  DA_INIT(af return_value, 1, af alloc);
  impact_t i1[] = { INTEGER_SET(h, 0, 43) };

  af return_value.values[0] = i1[0];
  AF_RETURN_VOID;
}
CORO_END

TEST("seq execution set value impact") {
  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);

  action_t action = ACTION(test_exe_alloc_set_, 1, HANDLE_NULL);

  REQUIRE(execution_queue(&exe, action) == KIT_OK);
  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);

  handle_t h = { .id = 0, .generation = 0 };
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  execution_destroy(&exe);
}

TEST("seq execution set value impact twice") {
  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);

  action_t action = ACTION(test_exe_alloc_set_, default_tick,
                           HANDLE_NULL);

  REQUIRE(execution_queue(&exe, action) == KIT_OK);
  handle_t h = { .id = 0, .generation = 0 };

  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  REQUIRE(execution_schedule_and_join(&exe, default_tick) == KIT_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 43);

  execution_destroy(&exe);
}

STATIC_CORO(impact_list_t, test_exe_alloc_set_continue_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  static handle_t const h0 = { .id = 0, .generation = -1 };
  static handle_t const h  = { .id = 0, .generation = 0 };

  DA_INIT(af return_value, 3, af alloc);
  impact_t i0[] = { INTEGER_ALLOCATE_INTO(h0, 1),
                    INTEGER_SET(h, 0, 42), TICK_CONTINUE() };

  af return_value.values[0] = i0[0];
  af return_value.values[1] = i0[1];
  af return_value.values[2] = i0[2];
  AF_YIELD_VOID;

  DA_INIT(af return_value, 1, af alloc);
  impact_t i1[] = { INTEGER_SET(h, 0, 43) };

  af return_value.values[0] = i1[0];
  AF_RETURN_VOID;
}
CORO_END

TEST("seq execution set value impact twice with continuation") {
  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);

  action_t action = ACTION(test_exe_alloc_set_continue_, default_tick,
                           HANDLE_NULL);

  REQUIRE(execution_queue(&exe, action) == KIT_OK);
  handle_t h = { .id = 0, .generation = 0 };

  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 43);

  REQUIRE(execution_schedule_and_join(&exe, default_tick) == KIT_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 43);

  execution_destroy(&exe);
}

STATIC_CORO(impact_list_t, test_exe_allocate_into_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  DA_INIT(af return_value, 1, af alloc);
  handle_t h   = { .id = 0, .generation = -1 };
  impact_t i[] = { INTEGER_ALLOCATE_INTO(h, 1) };

  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

STATIC_CORO(impact_list_t, test_exe_add_18_, kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  DA_INIT(af return_value, 1, af alloc);
  handle_t h   = { .id = 0, .generation = 0 };
  impact_t i[] = { INTEGER_ADD(h, 0, 18) };

  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

STATIC_CORO(impact_list_t, test_exe_add_24_, kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  DA_INIT(af return_value, 1, af alloc);
  handle_t h   = { .id = 0, .generation = 0 };
  impact_t i[] = { INTEGER_ADD(h, 0, 24) };

  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

TEST("seq execution two actions") {
  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);

  action_t a0 = ACTION(test_exe_allocate_into_, 1, HANDLE_NULL);
  REQUIRE(execution_queue(&exe, a0) == KIT_OK);

  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);

  action_t a1 = ACTION(test_exe_add_18_, 1, HANDLE_NULL);
  action_t a2 = ACTION(test_exe_add_24_, 1, HANDLE_NULL);
  REQUIRE(execution_queue(&exe, a1) == KIT_OK);
  REQUIRE(execution_queue(&exe, a2) == KIT_OK);

  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);

  handle_t h = { .id = 0, .generation = 0 };
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  execution_destroy(&exe);
}

STATIC_CORO(impact_list_t, test_exe_set_42_, kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  DA_INIT(af return_value, 1, af alloc);
  handle_t h   = { .id = 1, .generation = 0 };
  impact_t i[] = { INTEGER_SET(h, 0, 42) };

  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

TEST("seq execution invalid impact") {
  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);

  action_t action = ACTION(test_exe_set_42_, 1, HANDLE_NULL);
  REQUIRE(execution_queue(&exe, action) == KIT_OK);

  REQUIRE(execution_schedule_and_join(&exe, 1) ==
          ERROR_INVALID_HANDLE_ID);

  execution_destroy(&exe);
}

STATIC_CORO(impact_list_t, test_exe_0_alloc_set_42_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  DA_INIT(af return_value, 2, af alloc);
  handle_t h0  = { .id = 0, .generation = -1 };
  handle_t h   = { .id = 0, .generation = 0 };
  impact_t i[] = { INTEGER_SET(h, 0, 42),
                   INTEGER_ALLOCATE_INTO(h0, 1) };

  af return_value.values[0] = i[0];
  af return_value.values[1] = i[1];
  AF_RETURN_VOID;
}
CORO_END

STATIC_CORO(impact_list_t, test_exe_1_set_43_, kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  DA_INIT(af return_value, 1, af alloc);
  handle_t h   = { .id = 1, .generation = 0 };
  impact_t i[] = { INTEGER_SET(h, 0, 43) };

  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

STATIC_CORO(impact_list_t, test_exe_1_alloc_, kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  DA_INIT(af return_value, 1, af alloc);
  handle_t h   = { .id = 1, .generation = -1 };
  impact_t i[] = { INTEGER_ALLOCATE_INTO(h, 1) };

  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

TEST("seq execution sync impacts applied first") {
  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);

  action_t a[] = { ACTION_UNSAFE(test_exe_0_alloc_set_42_, 1,
                                 HANDLE_NULL),
                   ACTION_UNSAFE(test_exe_1_set_43_, 1, HANDLE_NULL),
                   ACTION_UNSAFE(test_exe_1_alloc_, 1, HANDLE_NULL) };
  REQUIRE(execution_queue(&exe, a[0]) == KIT_OK);
  REQUIRE(execution_queue(&exe, a[1]) == KIT_OK);
  REQUIRE(execution_queue(&exe, a[2]) == KIT_OK);

  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);

  handle_t h[] = { { .id = 0, .generation = 0 },
                   { .id = 1, .generation = 0 } };
  REQUIRE(state.get_integer(state.state, h[0], 0, -1) == 42);
  REQUIRE(state.get_integer(state.state, h[1], 0, -1) == 43);

  execution_destroy(&exe);
}

STATIC_CORO(impact_list_t, test_exe_queue_, kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  impact_t i[] = { QUEUE_ACTION(
      ACTION_UNSAFE(test_exe_0_alloc_set_42_, 1, HANDLE_NULL)) };

  DA_INIT(af return_value, 1, af alloc);
  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

TEST("seq execution queue action impact") {
  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);

  action_t a = ACTION(test_exe_queue_, 1, HANDLE_NULL);
  handle_t h = { .id = 0, .generation = 0 };

  REQUIRE(execution_queue(&exe, a) == KIT_OK);
  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  execution_destroy(&exe);
}

static ATOMIC(int) foo_begin;
static ATOMIC(int) foo_end;
static ATOMIC(int) bar_begin;
static ATOMIC(int) bar_end;

STATIC_CORO(impact_list_t, test_exe_foo_, kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  atomic_fetch_add_explicit(&foo_begin, 1, memory_order_acq_rel);

  struct timespec t = { .tv_sec = 0, .tv_nsec = 300000000 };
  thrd_sleep(&t, NULL);

  atomic_store_explicit(
      &foo_end,
      atomic_load_explicit(&bar_begin, memory_order_acquire),
      memory_order_release);

  impact_t i[] = { NOOP() };

  DA_INIT(af return_value, 1, af alloc);
  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

STATIC_CORO(impact_list_t, test_exe_bar_, kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  atomic_fetch_add_explicit(&bar_begin, 1, memory_order_acq_rel);

  struct timespec t = { .tv_sec = 0, .tv_nsec = 300000000 };
  thrd_sleep(&t, NULL);

  atomic_store_explicit(
      &bar_end,
      atomic_load_explicit(&foo_begin, memory_order_acquire),
      memory_order_release);

  impact_t i[] = { NOOP() };

  DA_INIT(af return_value, 1, af alloc);
  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

TEST("seq execution no multithreading") {
  atomic_store_explicit(&foo_begin, 0, memory_order_relaxed);
  atomic_store_explicit(&foo_end, 0, memory_order_relaxed);
  atomic_store_explicit(&bar_begin, 0, memory_order_relaxed);
  atomic_store_explicit(&bar_end, 0, memory_order_relaxed);

  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);

  action_t a[] = { ACTION_UNSAFE(test_exe_foo_, 1, HANDLE_NULL),
                   ACTION_UNSAFE(test_exe_bar_, 1, HANDLE_NULL) };

  REQUIRE(execution_queue(&exe, a[0]) == KIT_OK);
  REQUIRE(execution_queue(&exe, a[1]) == KIT_OK);
  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);

  REQUIRE(foo_end + bar_end == 1);

  execution_destroy(&exe);
}

TEST("execution two parallel actions") {
  atomic_store_explicit(&foo_begin, 0, memory_order_relaxed);
  atomic_store_explicit(&foo_end, 0, memory_order_relaxed);
  atomic_store_explicit(&bar_begin, 0, memory_order_relaxed);
  atomic_store_explicit(&bar_end, 0, memory_order_relaxed);

  kit_allocator_t alloc = kit_alloc_default();

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, alloc);
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  read_write_t state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == KIT_OK);

  action_t a[] = { ACTION_UNSAFE(test_exe_foo_, 1, HANDLE_NULL),
                   ACTION_UNSAFE(test_exe_bar_, 1, HANDLE_NULL) };

  REQUIRE(execution_queue(&exe, a[0]) == KIT_OK);
  REQUIRE(execution_queue(&exe, a[1]) == KIT_OK);
  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);

  REQUIRE(foo_end == 1);
  REQUIRE(bar_end == 1);

  execution_destroy(&exe);
}

STATIC_CORO(impact_list_t, test_exe_delay_alloc_set_42_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  struct timespec t = { .tv_sec = 0, .tv_nsec = 100000000 };
  thrd_sleep(&t, NULL);

  DA_INIT(af return_value, 2, af alloc);
  handle_t h0  = { .id = 0, .generation = -1 };
  handle_t h   = { .id = 0, .generation = 0 };
  impact_t i[] = { INTEGER_SET(h, 0, 42),
                   INTEGER_ALLOCATE_INTO(h0, 1) };

  af return_value.values[0] = i[0];
  af return_value.values[1] = i[1];
  AF_RETURN_VOID;
}
CORO_END

TEST("execution join one") {
  kit_allocator_t alloc = kit_alloc_default();

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, alloc);
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  read_write_t state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 1) == KIT_OK);

  action_t a = ACTION(test_exe_delay_alloc_set_42_, 1, HANDLE_NULL);
  handle_t h = { .id = 0, .generation = 0 };

  REQUIRE(execution_queue(&exe, a) == KIT_OK);
  REQUIRE(execution_schedule(&exe, 1) == KIT_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == -1);
  execution_join(&exe);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  execution_destroy(&exe);
}

TEST("execution join four") {
  kit_allocator_t alloc = kit_alloc_default();

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, alloc);
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  read_write_t state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == KIT_OK);

  action_t a = ACTION(test_exe_delay_alloc_set_42_, 1, HANDLE_NULL);
  handle_t h = { .id = 0, .generation = 0 };

  REQUIRE(execution_queue(&exe, a) == KIT_OK);
  REQUIRE(execution_schedule(&exe, 1) == KIT_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == -1);
  execution_join(&exe);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  execution_destroy(&exe);
}

TEST("execution set value impact") {
  kit_allocator_t alloc = kit_alloc_default();

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, alloc);
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  read_write_t state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == KIT_OK);

  action_t action = ACTION(test_exe_alloc_set_, 1, HANDLE_NULL);

  REQUIRE(execution_queue(&exe, action) == KIT_OK);
  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);

  handle_t h = { .id = 0, .generation = 0 };
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  execution_destroy(&exe);
}

TEST("execution set value impact twice") {
  kit_allocator_t alloc = kit_alloc_default();

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, alloc);
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  read_write_t state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == KIT_OK);

  action_t action = ACTION(test_exe_alloc_set_, default_tick,
                           HANDLE_NULL);

  REQUIRE(execution_queue(&exe, action) == KIT_OK);
  handle_t h = { .id = 0, .generation = 0 };

  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  REQUIRE(execution_schedule_and_join(&exe, default_tick) == KIT_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 43);

  execution_destroy(&exe);
}

TEST("execution set value impact twice with continuation") {
  kit_allocator_t alloc = kit_alloc_default();

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, alloc);
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  read_write_t state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == KIT_OK);

  action_t action = ACTION(test_exe_alloc_set_continue_, default_tick,
                           HANDLE_NULL);

  REQUIRE(execution_queue(&exe, action) == KIT_OK);
  handle_t h = { .id = 0, .generation = 0 };

  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 43);

  REQUIRE(execution_schedule_and_join(&exe, default_tick) == KIT_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 43);

  execution_destroy(&exe);
}

TEST("execution two actions") {
  kit_allocator_t alloc = kit_alloc_default();

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, alloc);
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  read_write_t state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == KIT_OK);

  action_t a0 = ACTION(test_exe_allocate_into_, 1, HANDLE_NULL);
  REQUIRE(execution_queue(&exe, a0) == KIT_OK);

  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);

  action_t a1 = ACTION(test_exe_add_18_, 1, HANDLE_NULL);
  action_t a2 = ACTION(test_exe_add_24_, 1, HANDLE_NULL);
  REQUIRE(execution_queue(&exe, a1) == KIT_OK);
  REQUIRE(execution_queue(&exe, a2) == KIT_OK);

  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);

  handle_t h = { .id = 0, .generation = 0 };
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  execution_destroy(&exe);
}

TEST("execution invalid impact") {
  kit_allocator_t alloc = kit_alloc_default();

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, alloc);
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  read_write_t state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == KIT_OK);

  action_t action = ACTION(test_exe_set_42_, 1, HANDLE_NULL);
  REQUIRE(execution_queue(&exe, action) == KIT_OK);

  REQUIRE(execution_schedule_and_join(&exe, 1) ==
          ERROR_INVALID_HANDLE_ID);

  execution_destroy(&exe);
}

TEST("execution sync impacts applied first") {
  kit_allocator_t alloc = kit_alloc_default();

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, alloc);
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  read_write_t state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == KIT_OK);

  action_t a[] = { ACTION_UNSAFE(test_exe_0_alloc_set_42_, 1,
                                 HANDLE_NULL),
                   ACTION_UNSAFE(test_exe_1_set_43_, 1, HANDLE_NULL),
                   ACTION_UNSAFE(test_exe_1_alloc_, 1, HANDLE_NULL) };
  REQUIRE(execution_queue(&exe, a[0]) == KIT_OK);
  REQUIRE(execution_queue(&exe, a[1]) == KIT_OK);
  REQUIRE(execution_queue(&exe, a[2]) == KIT_OK);

  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);

  handle_t h[] = { { .id = 0, .generation = 0 },
                   { .id = 1, .generation = 0 } };
  REQUIRE(state.get_integer(state.state, h[0], 0, -1) == 42);
  REQUIRE(state.get_integer(state.state, h[1], 0, -1) == 43);

  execution_destroy(&exe);
}

TEST("execution queue action impact") {
  kit_allocator_t alloc = kit_alloc_default();

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, alloc);
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  read_write_t state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == KIT_OK);

  action_t a = ACTION(test_exe_queue_, 1, HANDLE_NULL);
  handle_t h = { .id = 0, .generation = 0 };

  REQUIRE(execution_queue(&exe, a) == KIT_OK);
  REQUIRE(execution_schedule_and_join(&exe, 1) == KIT_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  execution_destroy(&exe);
}

STATIC_CORO(impact_list_t, test_exe_order_alloc_2_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  struct timespec t = { .tv_sec = 0, .tv_nsec = 20000000 };
  thrd_sleep(&t, NULL);

  handle_t h   = { .id = 0, .generation = -1 };
  impact_t i[] = { INTEGER_ALLOCATE_INTO(h, 1) };

  DA_INIT(af return_value, 1, af alloc);
  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

STATIC_CORO(impact_list_t, test_exe_order_alloc_1_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  struct timespec t = { .tv_sec = 0, .tv_nsec = 20000000 };
  thrd_sleep(&t, NULL);

  impact_t i[] = { QUEUE_ACTION(
      ACTION_UNSAFE(test_exe_order_alloc_2_, 1, HANDLE_NULL)) };

  DA_INIT(af return_value, 1, af alloc);
  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

STATIC_CORO(impact_list_t, test_exe_order_alloc_0_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  impact_t i0[] = { NOOP() };

  DA_INIT(af return_value, 1, af alloc);
  af return_value.values[0] = i0[0];
  AF_YIELD_VOID;

  struct timespec t = { .tv_sec = 0, .tv_nsec = 20000000 };
  thrd_sleep(&t, NULL);

  impact_t i1[] = { QUEUE_ACTION(
      ACTION_UNSAFE(test_exe_order_alloc_1_, 1, HANDLE_NULL)) };

  DA_INIT(af return_value, 1, af alloc);
  af return_value.values[0] = i1[0];
  AF_RETURN_VOID;
}
CORO_END

STATIC_CORO(impact_list_t, test_exe_order_dealloc_2_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  handle_t h   = { .id = 0, .generation = 0 };
  impact_t i[] = { INTEGER_DEALLOCATE(h) };

  DA_INIT(af return_value, 1, af alloc);
  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

STATIC_CORO(impact_list_t, test_exe_order_dealloc_1_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  impact_t i[] = { QUEUE_ACTION(
      ACTION_UNSAFE(test_exe_order_dealloc_2_, 1, HANDLE_NULL)) };

  DA_INIT(af return_value, 1, af alloc);
  af return_value.values[0] = i[0];
  AF_RETURN_VOID;
}
CORO_END

STATIC_CORO(impact_list_t, test_exe_order_dealloc_0_,
            kit_allocator_t alloc;
            read_only_t access; handle_t self;) {
  impact_t i0[] = { NOOP() };

  DA_INIT(af return_value, 1, af alloc);
  af return_value.values[0] = i0[0];
  AF_YIELD_VOID;

  impact_t i1[] = { QUEUE_ACTION(
      ACTION_UNSAFE(test_exe_order_dealloc_1_, 1, HANDLE_NULL)) };

  DA_INIT(af return_value, 1, af alloc);
  af return_value.values[0] = i1[0];
  AF_RETURN_VOID;
}
CORO_END

TEST("execution action order") {
  kit_allocator_t alloc = kit_alloc_default();

  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, alloc);
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  read_write_t state;
  REQUIRE(state_init(&state, alloc) == KIT_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == KIT_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == KIT_OK);

  action_t a[] = {
    ACTION_UNSAFE(test_exe_order_alloc_0_, 1, HANDLE_NULL),
    ACTION_UNSAFE(test_exe_order_dealloc_0_, 1, HANDLE_NULL)
  };

  REQUIRE(execution_queue(&exe, a[0]) == KIT_OK);
  REQUIRE(execution_queue(&exe, a[1]) == KIT_OK);

  REQUIRE(execution_schedule_and_join(&exe, 2) == KIT_OK);

  execution_destroy(&exe);
}

TEST("execution queue concurrency") {
  int ok = 1;

  for (int i = 0; i < 400; i++) {
    kit_allocator_t alloc = kit_alloc_default();

    pool_state_t_ pool_;
    DA_INIT(pool_.threads, 0, alloc);
    laplace_thread_pool_t pool = { .state   = &pool_,
                                   .release = pool_release_,
                                   .run     = pool_run_,
                                   .join    = pool_join_ };

    read_write_t state;
    ok = ok && (state_init(&state, alloc) == KIT_OK);

    execution_t exe;
    ok = ok && (execution_init(&exe, state, pool, alloc) == KIT_OK);
    ok = ok && (execution_set_thread_count(&exe, 4) == KIT_OK);

    action_t action = ACTION(test_exe_alloc_set_, 1, HANDLE_NULL);
    handle_t h      = { .id = 0, .generation = 0 };

    ok = ok && (execution_schedule(&exe, 1) == KIT_OK);

    ok = ok && (execution_queue(&exe, action) == KIT_OK);
    ok = ok && (state.get_integer(state.state, h, 0, -1) == -1);
    execution_join(&exe);
    ok = ok && (state.get_integer(state.state, h, 0, -1) == -1);
    ok = ok && (execution_schedule_and_join(&exe, 1) == KIT_OK);
    ok = ok && (state.get_integer(state.state, h, 0, -1) == 42);

    execution_destroy(&exe);
  }

  REQUIRE(ok);
}
