#include "execution.h"

#include <kit/lower_bound.h>
#include <kit/move_back.h>

#define LOCK_                                           \
  {                                                     \
    if (mtx_lock(&execution->_lock) != thrd_success) {  \
      execution->status = LAPLACE_ERROR_BAD_MUTEX_LOCK; \
      execution->_done  = 1;                            \
      return execution->status;                         \
    }                                                   \
  }

#define UNLOCK_                                           \
  {                                                       \
    if (mtx_unlock(&execution->_lock) != thrd_success) {  \
      execution->status = LAPLACE_ERROR_BAD_MUTEX_UNLOCK; \
      execution->_done  = 1;                              \
      return execution->status;                           \
    }                                                     \
  }

#define BROADCAST_(var_)                                    \
  if (cnd_broadcast(&execution->var_) != thrd_success) {    \
    (void) mtx_lock(&execution->_lock);                     \
    execution->status = LAPLACE_ERROR_BAD_CNDVAR_BROADCAST; \
    execution->_done  = 1;                                  \
    (void) mtx_unlock(&execution->_lock);                   \
    return execution->status;                               \
  }

#define WAIT_(var_)                                    \
  if (cnd_wait(&execution->var_, &execution->_lock) != \
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
      UNLOCK_                                                \
    BROADCAST_(_on_fence)

#define ONCE_END_                                               \
  LOCK_                                                         \
  }                                                             \
  else {                                                        \
    while (!execution->_done && execution->_fence_in != 0 &&    \
           execution->_fence_in != execution->thread_count)     \
      WAIT_(_on_fence)                                          \
  }                                                             \
  }                                                             \
  else {                                                        \
    UNLOCK_                                                     \
    BROADCAST_(_on_fence)                                       \
    LOCK_                                                       \
  }                                                             \
  if (!execution->_done) {                                      \
    if (++execution->_fence_out == execution->thread_count) {   \
      execution->_fence_in  = 0;                                \
      execution->_fence_out = 0;                                \
      UNLOCK_                                                   \
      BROADCAST_(_on_fence)                                     \
    } else {                                                    \
      while (!execution->_done && execution->_fence_out != 0 && \
             execution->_fence_out != execution->thread_count)  \
        WAIT_(_on_fence)                                        \
      UNLOCK_                                                   \
    }                                                           \
  } else {                                                      \
    UNLOCK_                                                     \
    BROADCAST_(_on_fence)                                       \
  }

static laplace_status_t sync_routine_(
    laplace_execution_t *const execution, laplace_time_t time) {
  for (; time != 0; time--) {
    for (int done = 0; !done;) {
      done = 1;

      for (ptrdiff_t i = 0; i < execution->_queue.size; i++) {
        laplace_action_state_t *action = execution->_queue.values + i;

        if (action->clock != 0 ||
            laplace_generator_status(&action->generator) ==
                LAPLACE_GENERATOR_FINISHED)
          continue;

        int is_continue = 0;

        laplace_impact_list_t list = laplace_generator_run(
            &action->generator);

        for (ptrdiff_t j = 0; j < list.size; j++) {
          laplace_impact_t *impact = list.values + j;

          switch (impact->type) {
            case LAPLACE_IMPACT_TICK_CONTINUE:
              is_continue = 1;
              done        = 0;
              break;

            case LAPLACE_IMPACT_QUEUE_ACTION: {
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

              ptrdiff_t n;
              LOWER_BOUND_INL(n, execution->_forks.size,
                              execution->_forks.values[index_].order <
                                  action->order);

              ptrdiff_t s = execution->_forks.size;
              DA_INSERT(execution->_forks, n, fork);
              if (execution->_forks.size != s + 1) {
                DA_DESTROY(list);
                return LAPLACE_ERROR_BAD_ALLOC;
              }

              done = 0;
            } break;

            default:
              if (laplace_impact_mode_of(impact) ==
                  LAPLACE_IMPACT_SYNC) {
                laplace_impact_t x = *impact;
                x.order            = action->order;

                ptrdiff_t n;
                LOWER_BOUND_INL(
                    n, execution->_sync.size,
                    execution->_sync.values[index_].order <
                        action->order);

                ptrdiff_t s = execution->_sync.size;
                DA_INSERT(execution->_sync, n, x);
                if (execution->_sync.size != s + 1) {
                  DA_DESTROY(list);
                  return LAPLACE_ERROR_BAD_ALLOC;
                }
              } else {
                ptrdiff_t const n = execution->_async.size;
                DA_RESIZE(execution->_async, n + 1);
                if (execution->_async.size != n + 1) {
                  DA_DESTROY(list);
                  return LAPLACE_ERROR_BAD_ALLOC;
                }

                execution->_async.values[n] = *impact;
              }
          }
        }

        DA_DESTROY(list);

        if (!is_continue)
          action->clock = action->tick_duration;
      }

      if (execution->_access.apply != NULL) {
        for (ptrdiff_t i = 0; i < execution->_sync.size; i++) {
          laplace_status_t s = execution->_access.apply(
              execution->_access.state, execution->_sync.values + i);
          if (s != LAPLACE_STATUS_OK)
            return s;
        }

        for (ptrdiff_t i = 0; i < execution->_async.size; i++) {
          laplace_status_t s = execution->_access.apply(
              execution->_access.state, execution->_async.values + i);
          if (s != LAPLACE_STATUS_OK)
            return s;
        }
      }

      for (ptrdiff_t i = 0; i < execution->_forks.size; i++)
        execution->_forks.values[i].order = execution->_queue.size +
                                            i;

      ptrdiff_t const n = execution->_queue.size;
      ptrdiff_t const s = execution->_forks.size;
      DA_RESIZE(execution->_queue, n + s);
      if (execution->_queue.size != n + s)
        return LAPLACE_ERROR_BAD_ALLOC;

      if (s != 0)
        memcpy(execution->_queue.values + n, execution->_forks.values,
               s * sizeof *execution->_queue.values);

      if (execution->_access.adjust_loop != NULL)
        execution->_access.adjust_loop(execution->_access.state);

      if (execution->_access.adjust_done != NULL)
        execution->_access.adjust_done(execution->_access.state);

      DA_RESIZE(execution->_sync, 0);
      DA_RESIZE(execution->_async, 0);
      DA_RESIZE(execution->_forks, 0);
    }

    MOVE_BACK_INL(execution->_queue.size, execution->_queue,
                  laplace_generator_status(
                      &execution->_queue.values[index_].generator) ==
                      LAPLACE_GENERATOR_FINISHED);

    for (ptrdiff_t i = 0; i < execution->_queue.size; i++) {
      execution->_queue.values[i].order = i;
      execution->_queue.values[i].clock--;
    }
  }

  return LAPLACE_STATUS_OK;
}

static laplace_status_t routine_internal_(
    laplace_execution_t *const execution) {
  LOCK_
  while (!execution->_done) {
    while (!execution->_done && execution->_ticks == 0)
      WAIT_(_on_tick)

    while (!execution->_done && execution->_ticks != 0) {
      UNLOCK_

      ONCE_BEGIN_
      execution->_tick_done = 0;
      ONCE_END_

      LOCK_
      while (!execution->_tick_done) {
        UNLOCK_

        ONCE_BEGIN_
        execution->_tick_done = 1;
        ONCE_END_

        LOCK_
        while (execution->_queue_index < execution->_queue.size) {
          ptrdiff_t i = execution->_queue_index++;
          UNLOCK_

          laplace_action_state_t *action = execution->_queue.values +
                                           i;

          if (action->clock != 0 ||
              laplace_generator_status(&action->generator) ==
                  LAPLACE_GENERATOR_FINISHED) {
            LOCK_
            continue;
          }

          int is_continue = 0;

          laplace_impact_list_t list = laplace_generator_run(
              &action->generator);

          for (ptrdiff_t j = 0; j < list.size; j++) {
            laplace_impact_t *impact = list.values + j;

            switch (impact->type) {
              case LAPLACE_IMPACT_TICK_CONTINUE:
                is_continue = 1;
                LOCK_
                execution->_tick_done = 0;
                UNLOCK_
                break;

              case LAPLACE_IMPACT_QUEUE_ACTION: {
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

                LOCK_
                ptrdiff_t n;
                ptrdiff_t s = execution->_forks.size;
                LOWER_BOUND_INL(
                    n, execution->_forks.size,
                    execution->_forks.values[index_].order <
                        action->order);
                DA_INSERT(execution->_forks, n, fork);
                int ok_ = execution->_forks.size == s + 1;
                UNLOCK_

                if (!ok_) {
                  DA_DESTROY(list);
                  return LAPLACE_ERROR_BAD_ALLOC;
                }

                LOCK_
                execution->_tick_done = 0;
                UNLOCK_
              } break;

              default:
                if (laplace_impact_mode_of(impact) ==
                    LAPLACE_IMPACT_SYNC) {
                  laplace_impact_t x = *impact;
                  x.order            = action->order;

                  LOCK_
                  ptrdiff_t n;
                  ptrdiff_t s = execution->_sync.size;
                  LOWER_BOUND_INL(
                      n, execution->_sync.size,
                      execution->_sync.values[index_].order <
                          action->order);
                  DA_INSERT(execution->_sync, n, x);
                  int ok_ = execution->_sync.size == s + 1;
                  UNLOCK_

                  if (!ok_) {
                    DA_DESTROY(list);
                    return LAPLACE_ERROR_BAD_ALLOC;
                  }

                } else {
                  LOCK_
                  ptrdiff_t const n = execution->_async.size;
                  DA_RESIZE(execution->_async, n + 1);
                  int ok_ = execution->_async.size == n + 1;
                  UNLOCK_

                  if (!ok_) {
                    DA_DESTROY(list);
                    return LAPLACE_ERROR_BAD_ALLOC;
                  }

                  execution->_async.values[n] = *impact;
                }
            }
          }

          DA_DESTROY(list);

          if (!is_continue)
            action->clock = action->tick_duration;

          LOCK_
        }
        UNLOCK_

        laplace_status_t res = LAPLACE_STATUS_OK;

        ONCE_BEGIN_ {
          execution->_queue_index = 0;

          if (execution->_access.apply != NULL)
            for (ptrdiff_t i = 0; i < execution->_sync.size; i++)
              res = execution->_access.apply(execution->_access.state,
                                             execution->_sync.values +
                                                 i);
          DA_RESIZE(execution->_sync, 0);
        }
        ONCE_END_

        if (res != LAPLACE_STATUS_OK)
          return res;

        LOCK_
        if (execution->_access.apply != NULL)
          while (execution->_async_index != execution->_async.size) {
            ptrdiff_t i = execution->_async_index++;
            UNLOCK_

            res = execution->_access.apply(execution->_access.state,
                                           execution->_async.values +
                                               i);

            if (res != LAPLACE_STATUS_OK)
              return res;

            LOCK_
          }
        UNLOCK_

        ONCE_BEGIN_ {
          execution->_async_index = 0;
          DA_RESIZE(execution->_async, 0);

          for (ptrdiff_t i = 0; i < execution->_forks.size; i++)
            execution->_forks.values[i].order =
                execution->_queue.size + i;
          ptrdiff_t const n = execution->_queue.size;
          ptrdiff_t const s = execution->_forks.size;
          DA_RESIZE(execution->_queue, n + s);
          if (execution->_queue.size != n + s)
            return LAPLACE_ERROR_BAD_ALLOC;
          if (s != 0)
            memcpy(execution->_queue.values + n,
                   execution->_forks.values,
                   s * sizeof *execution->_queue.values);
          DA_RESIZE(execution->_forks, 0);
        }
        ONCE_END_

        if (execution->_access.adjust_loop != NULL)
          execution->_access.adjust_loop(execution->_access.state);

        ONCE_BEGIN_
        if (execution->_access.adjust_done != NULL)
          execution->_access.adjust_done(execution->_access.state);
        ONCE_END_

        LOCK_
      }
      UNLOCK_

      ONCE_BEGIN_ {
        MOVE_BACK_INL(
            execution->_queue.size, execution->_queue,
            laplace_generator_status(
                &execution->_queue.values[index_].generator) ==
                LAPLACE_GENERATOR_FINISHED);

        LOCK_
        for (ptrdiff_t i = 0; i < execution->_queue.size; i++) {
          execution->_queue.values[i].order = i;
          execution->_queue.values[i].clock--;
        }
        int joined = (--execution->_ticks == 0);
        UNLOCK_

        if (joined)
          BROADCAST_(_on_join)
      }
      ONCE_END_

      LOCK_
    }
  }
  UNLOCK_

  return LAPLACE_STATUS_OK;
}

static int routine_(laplace_execution_t *const execution) {
  laplace_status_t s = routine_internal_(execution);

  if (s != LAPLACE_STATUS_OK) {
    LOCK_
    execution->status = s;
    execution->_done  = 1;
    UNLOCK_

    BROADCAST_(_on_fence);
    BROADCAST_(_on_tick);
    BROADCAST_(_on_join);
  }

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
    laplace_read_write_t const  access,
    laplace_thread_pool_t const thread_pool, kit_allocator_t alloc) {
  memset(execution, 0, sizeof *execution);

  if (mtx_init(&execution->_lock, mtx_plain) != thrd_success)
    return LAPLACE_ERROR_BAD_MUTEX_INIT;

  if (cnd_init(&execution->_on_tick) != thrd_success) {
    mtx_destroy(&execution->_lock);
    return LAPLACE_ERROR_BAD_CNDVAR_INIT;
  }

  if (cnd_init(&execution->_on_join) != thrd_success) {
    mtx_destroy(&execution->_lock);
    cnd_destroy(&execution->_on_tick);
    return LAPLACE_ERROR_BAD_CNDVAR_INIT;
  }

  if (cnd_init(&execution->_on_fence) != thrd_success) {
    mtx_destroy(&execution->_lock);
    cnd_destroy(&execution->_on_tick);
    cnd_destroy(&execution->_on_join);
    return LAPLACE_ERROR_BAD_CNDVAR_INIT;
  }

  if (access.acquire != NULL)
    access.acquire(access.state);

  if (thread_pool.acquire != NULL)
    thread_pool.acquire(thread_pool.state);

  execution->_access      = access;
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

  (void) cnd_broadcast(&execution->_on_tick);
  (void) cnd_broadcast(&execution->_on_fence);

  if (execution->_thread_pool.join != NULL)
    execution->_thread_pool.join(execution->_thread_pool.state);

  mtx_destroy(&execution->_lock);
  cnd_destroy(&execution->_on_tick);
  cnd_destroy(&execution->_on_join);
  cnd_destroy(&execution->_on_fence);

  if (execution->_access.release != NULL)
    execution->_access.release(execution->_access.state);

  if (execution->_thread_pool.release != NULL)
    execution->_thread_pool.release(execution->_thread_pool.state);

  DA_DESTROY(execution->_queue);
  DA_DESTROY(execution->_forks);
  DA_DESTROY(execution->_sync);
  DA_DESTROY(execution->_async);
}

laplace_status_t laplace_execution_set_thread_count(
    laplace_execution_t *const execution,
    ptrdiff_t const            thread_count) {
  if (execution->_thread_pool.run == NULL ||
      execution->_thread_pool.join == NULL)
    return LAPLACE_ERROR_NO_THREAD_POOL;

  if (thread_count < execution->thread_count) {
    LOCK_
    execution->_done = 1;
    UNLOCK_

    BROADCAST_(_on_tick)
    BROADCAST_(_on_fence)

    execution->_thread_pool.join(execution->_thread_pool.state);

    LOCK_
    execution->_done = 0;
    UNLOCK_

    if (thread_count != 0) {
      laplace_status_t const s = execution->_thread_pool.run(
          execution->_thread_pool.state, thread_count,
          (laplace_pool_routine_fn) routine_, execution);

      if (s != LAPLACE_STATUS_OK)
        return s;
    }
  } else {
    laplace_status_t const s = execution->_thread_pool.run(
        execution->_thread_pool.state,
        thread_count - execution->thread_count,
        (laplace_pool_routine_fn) routine_, execution);

    if (s != LAPLACE_STATUS_OK)
      return s;
  }

  execution->thread_count = thread_count;

  return LAPLACE_STATUS_OK;
}

laplace_read_only_t laplace_execution_read_only(
    laplace_execution_t const *const execution) {

  laplace_read_only_t read_only = {
    .state         = execution->_access.state,
    .acquire       = execution->_access.acquire,
    .release       = execution->_access.release,
    .integers_size = execution->_access.integers_size,
    .bytes_size    = execution->_access.bytes_size,
    .get_integer   = execution->_access.get_integer,
    .get_byte      = execution->_access.get_byte,
    .read_integers = execution->_access.read_integers,
    .read_bytes    = execution->_access.read_bytes
  };

  return read_only;
}

laplace_status_t laplace_execution_queue(
    laplace_execution_t *const execution,
    laplace_action_t const     action) {
  LOCK_

  ptrdiff_t const n = execution->_queue.size;

  DA_RESIZE(execution->_queue, n + 1);

  if (execution->_queue.size != n + 1) {
    UNLOCK_
    return LAPLACE_ERROR_BAD_ALLOC;
  }

  execution->_queue.values[n].order         = n;
  execution->_queue.values[n].clock         = execution->_ticks;
  execution->_queue.values[n].tick_duration = action.tick_duration;

  laplace_status_t s = laplace_generator_init(
      &execution->_queue.values[n].generator, action,
      laplace_execution_read_only(execution), execution->_alloc);

  UNLOCK_

  return s;
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

  return execution->status;
}
