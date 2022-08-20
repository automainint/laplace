#include "../../laplace/execution.h"

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
  pool_state_t_ *p = (pool_state_t_ *) state;
  ptrdiff_t      n = p->threads.size;
  for (ptrdiff_t i = size; i < n; i++)
    (void) thrd_join(p->threads.values[i], NULL);
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
  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  execution_t exe;
  REQUIRE(execution_init(&exe, pool, kit_alloc_default()) ==
          STATUS_OK);
  REQUIRE(exe.status == STATUS_OK);
  execution_destroy(&exe);
}

/*
TEST("execution get thread count") {
  laplace_thread_pool_t pool;
  memset(&pool, 0, sizeof pool);

  execution_t exe;
  REQUIRE(execution_init(&exe, pool, kit_alloc_default()) ==
          STATUS_OK);
  REQUIRE(exe.thread_count == 0);
  execution_destroy(&exe);
}

TEST("execution set thread count") {
  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, kit_alloc_default());
  laplace_thread_pool_t pool = { .state  = &pool_,
                                 .resize = pool_resize_,
                                 .join   = pool_join_ };

  execution_t exe;
  REQUIRE(execution_init(&exe, pool, kit_alloc_default()) ==
          STATUS_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == STATUS_OK);
  REQUIRE(exe.thread_count == 4);
  execution_destroy(&exe);
  DA_DESTROY(pool_.threads);
}

TEST("execution set thread count to zero") {
  pool_state_t_ pool_;
  DA_INIT(pool_.threads, 0, kit_alloc_default());
  laplace_thread_pool_t pool = { .state  = &pool_,
                                 .resize = pool_resize_,
                                 .join   = pool_join_ };

  execution_t exe;
  REQUIRE(execution_init(&exe, pool, kit_alloc_default()) ==
          STATUS_OK);
  REQUIRE(execution_set_thread_count(&exe, 4) == STATUS_OK);
  REQUIRE(execution_set_thread_count(&exe, 0) == STATUS_OK);
  REQUIRE(exe.thread_count == 0);
  execution_destroy(&exe);
  DA_DESTROY(pool_.threads);
}
*/
