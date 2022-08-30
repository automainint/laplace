#include "../../laplace/execution.h"
#include "../../laplace/state.h"

#define KIT_TEST_FILE execution
#include <kit_test/test.h>

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

static laplace_status_t pool_run_(void *state, ptrdiff_t count,
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
  laplace_thread_pool_t pool = { .state   = &pool_,
                                 .release = pool_release_,
                                 .run     = pool_run_,
                                 .join    = pool_join_ };

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, kit_alloc_default()) ==
          STATUS_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == STATUS_OK);
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
          STATUS_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == STATUS_OK);
  REQUIRE(execution_set_thread_count(&exe, 0) == STATUS_OK);
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
  REQUIRE(state_init(&state, alloc) == STATUS_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == STATUS_OK);

  action_t action = ACTION(test_exe_alloc_set_, 1, HANDLE_NULL);

  REQUIRE(execution_queue(&exe, action) == STATUS_OK);
  REQUIRE(execution_schedule_and_join(&exe, 1) == STATUS_OK);

  handle_t h = { .id = 0, .generation = 0 };
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  execution_destroy(&exe);
}

TEST("seq execution set value impact twice") {
  const laplace_time_t tick = 10;

  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == STATUS_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == STATUS_OK);

  action_t action = ACTION(test_exe_alloc_set_, tick, HANDLE_NULL);

  REQUIRE(execution_queue(&exe, action) == STATUS_OK);
  handle_t h = { .id = 0, .generation = 0 };

  REQUIRE(execution_schedule_and_join(&exe, 1) == STATUS_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  REQUIRE(execution_schedule_and_join(&exe, tick) == STATUS_OK);
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
  const laplace_time_t tick = 10;

  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  kit_allocator_t alloc = kit_alloc_default();
  read_write_t    state;
  REQUIRE(state_init(&state, alloc) == STATUS_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == STATUS_OK);

  action_t action = ACTION(test_exe_alloc_set_continue_, tick,
                           HANDLE_NULL);

  REQUIRE(execution_queue(&exe, action) == STATUS_OK);
  handle_t h = { .id = 0, .generation = 0 };

  REQUIRE(execution_schedule_and_join(&exe, 1) == STATUS_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 43);

  REQUIRE(execution_schedule_and_join(&exe, tick) == STATUS_OK);
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
  REQUIRE(state_init(&state, alloc) == STATUS_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == STATUS_OK);

  action_t a0 = ACTION(test_exe_allocate_into_, 1, HANDLE_NULL);
  REQUIRE(execution_queue(&exe, a0) == STATUS_OK);

  REQUIRE(execution_schedule_and_join(&exe, 1) == STATUS_OK);

  action_t a1 = ACTION(test_exe_add_18_, 1, HANDLE_NULL);
  action_t a2 = ACTION(test_exe_add_24_, 1, HANDLE_NULL);
  REQUIRE(execution_queue(&exe, a1) == STATUS_OK);
  REQUIRE(execution_queue(&exe, a2) == STATUS_OK);

  REQUIRE(execution_schedule_and_join(&exe, 1) == STATUS_OK);

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
  REQUIRE(state_init(&state, alloc) == STATUS_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == STATUS_OK);

  action_t action = ACTION(test_exe_set_42_, 1, HANDLE_NULL);
  REQUIRE(execution_queue(&exe, action) == STATUS_OK);

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
  REQUIRE(state_init(&state, alloc) == STATUS_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == STATUS_OK);

  action_t a[] = { ACTION_UNSAFE(test_exe_0_alloc_set_42_, 1,
                                 HANDLE_NULL),
                   ACTION_UNSAFE(test_exe_1_set_43_, 1, HANDLE_NULL),
                   ACTION_UNSAFE(test_exe_1_alloc_, 1, HANDLE_NULL) };
  REQUIRE(execution_queue(&exe, a[0]) == STATUS_OK);
  REQUIRE(execution_queue(&exe, a[1]) == STATUS_OK);
  REQUIRE(execution_queue(&exe, a[2]) == STATUS_OK);

  REQUIRE(execution_schedule_and_join(&exe, 1) == STATUS_OK);

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
  REQUIRE(state_init(&state, alloc) == STATUS_OK);

  execution_t exe;
  REQUIRE(execution_init(&exe, state, pool, alloc) == STATUS_OK);

  action_t a = ACTION(test_exe_queue_, 1, HANDLE_NULL);
  handle_t h = { .id = 0, .generation = 0 };

  REQUIRE(execution_queue(&exe, a) == STATUS_OK);
  REQUIRE(execution_schedule_and_join(&exe, 1) == STATUS_OK);
  REQUIRE(state.get_integer(state.state, h, 0, -1) == 42);

  execution_destroy(&exe);
}

/*
TEST("x seq execution no multithreading") {
  static auto m         = std::mutex {};
  static auto foo_begin = 0;
  static auto foo_end   = 0;
  static auto bar_begin = 0;
  static auto bar_end   = 0;

  auto exe = laplace::execution {}.set_thread_count(0);

  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        m.lock();
        foo_begin++;
        m.unlock();
        sleep_for(milliseconds(300));
        m.lock();
        foo_end = bar_begin;
        m.unlock();
        co_yield laplace::impact { laplace::noop {} };
      }));
  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        m.lock();
        bar_begin++;
        m.unlock();
        sleep_for(milliseconds(300));
        m.lock();
        bar_end = foo_begin;
        m.unlock();
        co_yield laplace::impact { laplace::noop {} };
      }));
  exe.schedule_and_join(1);

  REQUIRE(foo_end + bar_end == 1);
}

TEST("x execution two parallel actions") {
  static auto m         = std::mutex {};
  static auto foo_begin = 0;
  static auto foo_end   = 0;
  static auto bar_begin = 0;
  static auto bar_end   = 0;

  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        m.lock();
        foo_begin++;
        m.unlock();
        sleep_for(milliseconds(300));
        m.lock();
        foo_end = bar_begin;
        m.unlock();
        co_yield laplace::impact { laplace::noop {} };
      }));
  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        m.lock();
        bar_begin++;
        m.unlock();
        sleep_for(milliseconds(300));
        m.lock();
        bar_end = foo_begin;
        m.unlock();
        co_yield laplace::impact { laplace::noop {} };
      }));
  exe.schedule_and_join(1);

  REQUIRE(foo_end == 1);
  REQUIRE(bar_end == 1);
}

TEST("x execution join one") {
  auto exe = laplace::execution {}.set_thread_count(1);

  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        sleep_for(milliseconds(100));
        co_yield laplace::impact {
          laplace::integer_allocate_into { .id = 0, .size = 1 }
        } + laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } };
      }));

  exe.schedule(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == -1);
  exe.join();
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("x execution join four") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [&](laplace::entity) -> coro::generator<laplace::impact_list> {
        sleep_for(milliseconds(100));
        co_yield laplace::impact {
          laplace::integer_allocate_into { .id = 0, .size = 1 }
        } + laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } };
      }));

  exe.schedule(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == -1);
  exe.join();
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("x execution set value impact") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact {
          laplace::integer_allocate_into { .id = 0, .size = 1 }
        } + laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } };
        co_yield laplace::impact { laplace::integer_set {
            .id = 0, .index = 0, .value = 43 } };
      }));

  exe.schedule_and_join(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("x execution set value impact twice") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact {
          laplace::integer_allocate_into { .id = 0, .size = 1 }
        } + laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } };
        co_yield laplace::impact { laplace::integer_set {
            .id = 0, .index = 0, .value = 43 } };
      }));

  const auto tick = laplace::action::default_tick_duration;

  exe.schedule_and_join(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);

  exe.schedule_and_join(tick);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 43);
}

TEST("x execution set value impact twice with continuation") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_allocate_into {
            .id = 0, .size = 1 } } +
            laplace::impact { laplace::integer_set {
                .id = 0, .index = 0, .value = 42 } } +
            laplace::impact { laplace::tick_continue {} };
        co_yield laplace::impact { laplace::integer_set {
            .id = 0, .index = 0, .value = 43 } };
      }));

  const auto tick = laplace::action::default_tick_duration;

  exe.schedule_and_join(1);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 43);

  exe.schedule_and_join(tick);
  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 43);
}

TEST("x execution two actions") {
  auto exe = laplace::execution {}.set_thread_count(4);

  const auto tick = laplace::action::default_tick_duration;

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_allocate_into {
            .id = 0, .size = 1 } };
      }));
  exe.schedule_and_join(1);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_add {
            .id = 0, .index = 0, .delta = 18 } };
      }));
  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_add {
            .id = 0, .index = 0, .delta = 24 } };
      }));
  exe.schedule_and_join(tick);

  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("x execution invalid impact") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_set {
            .id = 1, .index = 0, .value = 42 } };
      }));
  exe.schedule_and_join(1);

  REQUIRE(exe.error());
}

TEST("x execution sync impacts applied first") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact {
          laplace::integer_set { .id = 0, .index = 0, .value = 42 }
        } + laplace::impact { laplace::integer_allocate_into {
                .id = 0, .size = 1 } };
      }));
  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_set {
            .id = 1, .index = 0, .value = 43 } };
      }));
  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::integer_allocate_into {
            .id = 1, .size = 1 } };
      }));
  exe.schedule_and_join(1);

  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
  REQUIRE(exe.read_only().get_integer(1, 0, -1) == 43);
}

TEST("x execution queue action impact") {
  auto exe = laplace::execution {}.set_thread_count(4);

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::queue_action {
            laplace::action {}.setup(
                [](laplace::entity)
                    -> coro::generator<laplace::impact_list> {
                  co_yield laplace::impact {
                    laplace::integer_allocate_into { .id   = 0,
                                                     .size = 1 }
                  } + laplace::impact { laplace::integer_set {
                          .id = 0, .index = 0, .value = 42 } };
                }) } };
      }));

  exe.schedule_and_join(1);

  REQUIRE(exe.read_only().get_integer(0, 0, -1) == 42);
}

TEST("x execution action order") {
  auto       exe  = laplace::execution {}.set_thread_count(4);
  const auto tick = laplace::action::default_tick_duration;

  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::noop {} };
        sleep_for(milliseconds(20));
        co_yield laplace::impact {
          laplace::queue_action { laplace::action {}.setup(
              { [](laplace::entity)
                    -> coro::generator<laplace::impact_list> {
                sleep_for(milliseconds(20));
                co_yield laplace::impact {
                  laplace::queue_action { laplace::action {}.setup(
                      { [](laplace::entity)
                            -> coro::generator<laplace::impact_list> {
                        sleep_for(milliseconds(20));
                        co_yield laplace::impact {
                          laplace::integer_allocate_into { .id   = 0,
                                                           .size = 1 }
                        };
                      } }) }
                };
              } }) }
        };
      }));
  exe.queue(laplace::action {}.setup(
      [](laplace::entity) -> coro::generator<laplace::impact_list> {
        co_yield laplace::impact { laplace::noop {} };
        co_yield laplace::impact {
          laplace::queue_action { laplace::action {}.setup(
              { [](laplace::entity)
                    -> coro::generator<laplace::impact_list> {
                co_yield laplace::impact {
                  laplace::queue_action { laplace::action {}.setup(
                      { [](laplace::entity)
                            -> coro::generator<laplace::impact_list> {
                        co_yield laplace::impact {
                          laplace::integer_deallocate { .id = 0 }
                        };
                      } }) }
                };
              } }) }
        };
      }));
  exe.schedule_and_join(1 + tick);

  REQUIRE(!exe.error());
}
 */
