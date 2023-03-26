#ifndef LAPLACE_GENERATOR_H
#define LAPLACE_GENERATOR_H

#include "impact.h"
#include "promise.h"

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  LAPLACE_GENERATOR_RUNNING  = 0,
  LAPLACE_GENERATOR_FINISHED = 1
} laplace_generator_status_t;

typedef struct {
  union {
    laplace_promise_t promise;
    struct {
      char _padding[LAPLACE_COROUTINE_SIZE];
    };
  };
  laplace_time_t tick_duration;
  laplace_time_t clock;
} laplace_generator_t;

kit_status_t laplace_generator_init(laplace_generator_t *generator,
                                    laplace_action_t     action,
                                    laplace_read_only_t  access,
                                    kit_allocator_t      alloc);

laplace_impact_list_t laplace_generator_run(
    laplace_generator_t *generator);

laplace_generator_status_t laplace_generator_status(
    laplace_generator_t const *generator);

#define LAPLACE_ACTION_UNSAFE(id_, coro_, tick_duration_, ...)  \
  {                                                             \
    .id = id_, .size = sizeof(AF_TYPE(coro_)), .coro = (coro_), \
    .tick_duration = (tick_duration_), .self = __VA_ARGS__      \
  }

#define LAPLACE_ACTION(id_, coro_, tick_duration_, ...)             \
  LAPLACE_ACTION_UNSAFE(id_, coro_, (tick_duration_), __VA_ARGS__); \
  static_assert(offsetof(AF_TYPE(coro_), return_value) ==           \
                    offsetof(laplace_promise_t, return_value),      \
                "Wrong return_value offset");                       \
  static_assert(offsetof(AF_TYPE(coro_), action_id) ==              \
                    offsetof(laplace_promise_t, action_id),         \
                "Wrong action_id offset");                          \
  static_assert(offsetof(AF_TYPE(coro_), alloc) ==                  \
                    offsetof(laplace_promise_t, alloc),             \
                "Wrong alloc offset");                              \
  static_assert(offsetof(AF_TYPE(coro_), access) ==                 \
                    offsetof(laplace_promise_t, access),            \
                "Wrong access offset");                             \
  static_assert(offsetof(AF_TYPE(coro_), self) ==                   \
                    offsetof(laplace_promise_t, self),              \
                "Wrong self offset")

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define GENERATOR_RUNNING LAPLACE_GENERATOR_RUNNING
#  define GENERATOR_FINISHED LAPLACE_GENERATOR_FINISHED
#  define ACTION_UNSAFE LAPLACE_ACTION_UNSAFE
#  define ACTION LAPLACE_ACTION

#  define generator_t laplace_generator_t

#  define generator_init laplace_generator_init
#  define generator_run laplace_generator_run
#  define generator_status laplace_generator_status
#endif

#ifdef __cplusplus
}
#endif

#endif
