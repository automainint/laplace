#ifndef LAPLACE_EXECUTION_H
#define LAPLACE_EXECUTION_H

#include "access.h"
#include "generator.h"
#include "impact.h"

#include <kit/allocator.h>
#include <kit/atomic.h>
#include <kit/condition_variable.h>
#include <kit/mutex.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct laplace_execution laplace_execution_t;

typedef int (*laplace_pool_routine_fn)(void *execution);

typedef laplace_status_t (*laplace_pool_run_fn)(
    void *state, ptrdiff_t count, laplace_pool_routine_fn routine,
    laplace_execution_t *execution);

typedef void (*laplace_pool_join_fn)(void *state);

typedef struct {
  void                *state;
  laplace_acquire_fn   acquire;
  laplace_release_fn   release;
  laplace_pool_run_fn  run;
  laplace_pool_join_fn join;
} laplace_thread_pool_t;

typedef struct {
  ptrdiff_t           order;
  laplace_time_t      clock;
  laplace_time_t      tick_duration;
  laplace_generator_t generator;
} laplace_action_state_t;

struct laplace_execution {
  laplace_status_t status;
  ptrdiff_t        thread_count;

  laplace_read_write_t  _access;
  laplace_thread_pool_t _thread_pool;
  kit_allocator_t       _alloc;

  KIT_DA(_queue, laplace_action_state_t);
  KIT_DA(_forks, laplace_action_state_t);
  KIT_DA(_sync, laplace_impact_t);
  KIT_DA(_async, laplace_impact_t);

  int            _done;
  int            _tick_done;
  laplace_time_t _ticks;
  ptrdiff_t      _fence_in;
  ptrdiff_t      _fence_out;
  ptrdiff_t      _queue_index;
  ptrdiff_t      _async_index;

  mtx_t _lock;
  cnd_t _on_tick;
  cnd_t _on_join;
  cnd_t _on_fence;
};

laplace_status_t laplace_execution_init(
    laplace_execution_t *execution, laplace_read_write_t access,
    laplace_thread_pool_t thread_pool, kit_allocator_t alloc);

void laplace_execution_destroy(laplace_execution_t *execution);

laplace_status_t laplace_execution_set_thread_count(
    laplace_execution_t *execution, ptrdiff_t thread_count);

laplace_read_only_t laplace_execution_read_only(
    laplace_execution_t const *execution);

laplace_status_t laplace_execution_queue(
    laplace_execution_t *execution, laplace_action_t action);

laplace_status_t laplace_execution_schedule(
    laplace_execution_t *execution, laplace_time_t time);

void laplace_execution_join(laplace_execution_t *execution);

laplace_status_t laplace_execution_schedule_and_join(
    laplace_execution_t *execution, laplace_time_t time);

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define execution_t laplace_execution_t
#  define pool_routine_fn laplace_pool_routine_fn
#  define pool_resize_fn laplace_pool_resize_fn
#  define pool_join_fn laplace_pool_join_fn
#  define thread_pool_t laplace_thread_pool_t

#  define execution_init laplace_execution_init
#  define execution_destroy laplace_execution_destroy
#  define execution_set_thread_count \
    laplace_execution_set_thread_count
#  define execution_read_only laplace_execution_read_only
#  define execution_queue laplace_execution_queue
#  define execution_schedule laplace_execution_schedule
#  define execution_join laplace_execution_join
#  define execution_schedule_and_join \
    laplace_execution_schedule_and_join
#endif

#ifdef __cplusplus
}
#endif

#endif
