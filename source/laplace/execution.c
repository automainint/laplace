#include "execution.h"

#include <kit/lower_bound.h>

#define LOCK_                                         \
  if (mtx_lock(&execution->_lock) != thrd_success) {  \
    execution->status = LAPLACE_ERROR_BAD_MUTEX_LOCK; \
    execution->_done  = 1;                            \
    return execution->status;                         \
  }

#define UNLOCK_                                         \
  if (mtx_unlock(&execution->_lock) != thrd_success) {  \
    execution->status = LAPLACE_ERROR_BAD_MUTEX_UNLOCK; \
    execution->_done  = 1;                              \
    return execution->status;                           \
  }

#define BROADCAST_(_var)                                    \
  if (cnd_broadcast(&execution->_var) != thrd_success) {    \
    (void) mtx_lock(&execution->_lock);                     \
    execution->status = LAPLACE_ERROR_BAD_CNDVAR_BROADCAST; \
    execution->_done  = 1;                                  \
    (void) mtx_unlock(&execution->_lock);                   \
    return execution->status;                               \
  }

#define WAIT_(_var)                                    \
  if (cnd_wait(&execution->_var, &execution->_lock) != \
      thrd_success) {                                  \
    (void) mtx_lock(&execution->_lock);                \
    execution->status = LAPLACE_ERROR_BAD_CNDVAR_WAIT; \
    execution->_done  = 1;                             \
    (void) mtx_unlock(&execution->_lock);              \
    return execution->status;                          \
  }

#define ONCE_BEGIN_                                          \
  LOCK_                                                      \
  if (!execution->_done) {                                   \
    if (++execution->_fence_in == execution->thread_count) { \
    UNLOCK_

#define ONCE_END_                                                 \
  BROADCAST_(_on_fence)                                           \
  LOCK_                                                           \
  }                                                               \
  else {                                                          \
    do {                                                          \
      WAIT_(_on_fence)                                            \
    } while (!execution->_done &&                                 \
             execution->_fence_in != execution->thread_count);    \
  }                                                               \
  }                                                               \
  if (!execution->_done) {                                        \
    if (++execution->_fence_out == execution->thread_count) {     \
      execution->_fence_in  = 0;                                  \
      execution->_fence_out = 0;                                  \
      UNLOCK_                                                     \
      BROADCAST_(_on_fence)                                       \
    } else {                                                      \
      do {                                                        \
        WAIT_(_on_fence)                                          \
      } while (!execution->_done &&                               \
               execution->_fence_out != execution->thread_count); \
      UNLOCK_                                                     \
    }                                                             \
  }

int action_state_less_(laplace_action_state_t *left,
                       ptrdiff_t               right) {
  return left->order < right;
}

int impact_less_(laplace_impact_t *left, ptrdiff_t right) {
  return left->order < right;
}

static laplace_status_t sync_routine_(
    laplace_execution_t *const execution, laplace_time_t time) {
  for (; time != 0; time--) {
    for (int done = 0; !done;) {
      done = 1;

      for (ptrdiff_t i = 0; i < execution->_queue.size; i++) {
        laplace_action_state_t *action = execution->_queue.values + i;

        if (laplace_generator_status(action->generator) ==
                LAPLACE_GENERATOR_FINISHED ||
            action->clock != 0)
          continue;

        int is_continue = 0;

        laplace_impact_list_t list = laplace_generator_run(
            action->generator);

        for (ptrdiff_t j = 0; j < list.size; j++) {
          laplace_impact_t *impact = list.values + j;

          switch (impact->type) {
            case LAPLACE_IMPACT_TICK_CONTINUE:
              is_continue = 1;
              done        = 0;
              break;

            case LAPLACE_IMPACT_QUEUE_ACTION: {
              ptrdiff_t n;
              LOWER_BOUND_REF(n, execution->_forks, action->order,
                              action_state_less_);

              laplace_action_state_t fork = {
                .order = 0,
                .clock = 0,
                .tick_duration =
                    impact->queue_action.action.tick_duration
              };

              laplace_generator_init(
                  &fork.generator, impact->queue_action.action,
                  laplace_execution_read_only(execution),
                  execution->_alloc);

              ptrdiff_t s = execution->_forks.size;
              DA_INSERT(execution->_forks, n, fork);
              if (execution->_forks.size != s + 1)
                return LAPLACE_ERROR_BAD_ALLOC;
            } break;

            default:
              if (laplace_impact_mode_of(impact) ==
                  LAPLACE_IMPACT_SYNC) {
                ptrdiff_t n;
                LOWER_BOUND_REF(n, execution->_sync, action->order,
                                impact_less_);

                laplace_impact_t x = *impact;
                x.order            = action->order;

                ptrdiff_t s = execution->_sync.size;
                DA_INSERT(execution->_sync, n, x);
                if (execution->_sync.size != s + 1)
                  return LAPLACE_ERROR_BAD_ALLOC;
              } else {
                ptrdiff_t const n = execution->_async.size;
                DA_RESIZE(execution->_async, n + 1);
                if (execution->_async.size != n + 1)
                  return LAPLACE_ERROR_BAD_ALLOC;

                execution->_async.values[n] = *impact;
              }
          }
        }

        if (!is_continue)
          action->clock = action->tick_duration;
      }

      for (ptrdiff_t i = 0; i < execution->_sync.size; i++) {
        laplace_status_t s = execution->access.apply(
            execution->access.state, execution->_sync.values + i);
        if (s != LAPLACE_STATUS_OK)
          return s;
      }

      for (ptrdiff_t i = 0; i < execution->_async.size; i++) {
        laplace_status_t s = execution->access.apply(
            execution->access.state, execution->_async.values + i);
        if (s != LAPLACE_STATUS_OK)
          return s;
      }

      for (ptrdiff_t i = 0; i < execution->_forks.size; i++)
        execution->_forks.values[i].order = execution->_queue.size +
                                            i;

      ptrdiff_t const n = execution->_queue.size;
      ptrdiff_t const s = execution->_forks.size;
      DA_RESIZE(execution->_queue, n + s);
      if (execution->_queue.size != n + s)
        return LAPLACE_ERROR_BAD_ALLOC;

      memcpy(execution->_queue.values + n, execution->_forks.values,
             s);

      DA_RESIZE(execution->_sync, 0);
      DA_RESIZE(execution->_async, 0);
      DA_RESIZE(execution->_forks, 0);
    }

    /*  FIXME
     *  Move this algorithm to kit.
     */
    ptrdiff_t n = execution->_queue.size;
    for (ptrdiff_t i = 0; i < n;) {
      if (laplace_generator_status(
              execution->_queue.values[i].generator) ==
          GENERATOR_FINISHED) {
        n--;
        if (i < n) {
          laplace_generator_destroy(
              execution->_queue.values[i].generator);
          execution->_queue.values[i] = execution->_queue.values[n];
        }
      } else
        i++;
    }
    DA_RESIZE(execution->_queue, n);

    for (ptrdiff_t i = 0; i < execution->_queue.size; i++) {
      execution->_queue.values[i].order = i;
      execution->_queue.values[i].clock--;
    }
  }

  return LAPLACE_STATUS_OK;
}

static int routine_(laplace_execution_t *const execution) {
  // ONCE_BEGIN_
  // ONCE_END_
  LOCK_
  while (!execution->_done) {
    UNLOCK_
    thrd_yield();
    LOCK_
  }
  UNLOCK_
  return 0;
}

static laplace_status_t append_tick_(
    laplace_execution_t *const execution, laplace_time_t const time) {
  if (mtx_lock(&execution->_lock) != thrd_success)
    return LAPLACE_ERROR_BAD_MUTEX_LOCK;
  execution->_ticks += time;
  (void) mtx_unlock(&execution->_lock);

  if (cnd_broadcast(&execution->_on_tick) != thrd_success)
    return LAPLACE_ERROR_BAD_CNDVAR_BROADCAST;

  return LAPLACE_STATUS_OK;
}

laplace_status_t laplace_execution_init(
    laplace_execution_t *const  execution,
    laplace_thread_pool_t const thread_pool, kit_allocator_t alloc) {
  memset(execution, 0, sizeof *execution);

  if (mtx_init(&execution->_lock, mtx_plain) != thrd_success)
    return LAPLACE_ERROR_BAD_MUTEX_LOCK;

  execution->_thread_pool = thread_pool;
  execution->_alloc       = alloc;

  DA_INIT(execution->_queue, 0, alloc);
  DA_INIT(execution->_forks, 0, alloc);
  DA_INIT(execution->_sync, 0, alloc);
  DA_INIT(execution->_async, 0, alloc);

  return LAPLACE_STATUS_OK;
}

void laplace_execution_destroy(laplace_execution_t *const execution) {
  if (mtx_lock(&execution->_lock) == thrd_success) {
    execution->_done = 1;
    (void) mtx_unlock(&execution->_lock);
  }

  if (execution->_thread_pool.join != NULL)
    execution->_thread_pool.join(execution->_thread_pool.state);

  mtx_destroy(&execution->_lock);

  for (ptrdiff_t i = 0; i < execution->_queue.size; i++)
    laplace_generator_destroy(execution->_queue.values[i].generator);

  for (ptrdiff_t i = 0; i < execution->_forks.size; i++)
    laplace_generator_destroy(execution->_queue.values[i].generator);

  DA_DESTROY(execution->_queue);
  DA_DESTROY(execution->_forks);
  DA_DESTROY(execution->_sync);
  DA_DESTROY(execution->_async);
}

laplace_status_t laplace_execution_set_thread_count(
    laplace_execution_t *const execution,
    ptrdiff_t const            thread_count) {

  if (thread_count == 0) {
    LOCK_
    execution->_done = 1;
    UNLOCK_
  }

  laplace_status_t const s = execution->_thread_pool.resize(
      execution->_thread_pool.state, thread_count,
      (laplace_pool_routine_fn) routine_, execution);

  if (s != LAPLACE_STATUS_OK)
    return s;

  if (thread_count == 0) {
    LOCK_
    execution->_done = 0;
    UNLOCK_
  }

  execution->thread_count = thread_count;

  return LAPLACE_STATUS_OK;
}

laplace_read_only_t laplace_execution_read_only(
    laplace_execution_t const *const execution) {

  laplace_read_only_t read_only = {
    .state         = execution->access.state,
    .acquire       = execution->access.acquire,
    .release       = execution->access.release,
    .integers_size = execution->access.integers_size,
    .bytes_size    = execution->access.bytes_size,
    .get_integer   = execution->access.get_integer,
    .get_byte      = execution->access.get_byte,
    .read_integers = execution->access.read_integers,
    .read_bytes    = execution->access.read_bytes
  };

  return read_only;
}

laplace_status_t laplace_execution_queue(
    laplace_execution_t *const execution,
    laplace_action_t const     action) {
  ptrdiff_t const n = execution->_queue.size;

  DA_RESIZE(execution->_queue, n + 1);
  if (execution->_queue.size != n + 1)
    return LAPLACE_ERROR_BAD_ALLOC;

  execution->_queue.values[n].order         = 0;
  execution->_queue.values[n].clock         = 0;
  execution->_queue.values[n].tick_duration = action.tick_duration;

  laplace_generator_init(
      &execution->_queue.values[n].generator, action,
      laplace_execution_read_only(execution), execution->_alloc);

  return LAPLACE_STATUS_OK;
}

laplace_status_t laplace_execution_schedule(
    laplace_execution_t *execution, laplace_time_t time) {
  if (execution->thread_count == 0)
    return sync_routine_(execution, time);

  return append_tick_(execution, time);
}

void laplace_execution_join(laplace_execution_t *execution) {
  if (execution->thread_count == 0)
    return;

  if (mtx_lock(&execution->_lock) != thrd_success)
    return;
  while (!execution->_done && execution->_ticks != 0 &&
         cnd_wait(&execution->_on_join, &execution->_lock) ==
             thrd_success) { }
  (void) mtx_unlock(&execution->_lock);
}

laplace_status_t laplace_execution_schedule_and_join(
    laplace_execution_t *const execution, laplace_time_t const time) {
  laplace_status_t const s = laplace_execution_schedule(execution,
                                                        time);

  if (s != LAPLACE_STATUS_OK)
    return s;

  laplace_execution_join(execution);

  return LAPLACE_STATUS_OK;
}
