#ifndef LAPLACE_ACTION_H
#define LAPLACE_ACTION_H

#include "access.h"
#include "handle.h"
#include <kit/allocator.h>
#include <kit/async_function.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct laplace_promise laplace_promise_t;

typedef struct {
  ptrdiff_t            size;
  kit_af_state_machine coro;
  laplace_time_t       tick_duration;
  laplace_handle_t     self;
} laplace_action_t;

#define LAPLACE_ACTION(coro_, tick_duration_, self_)     \
  { .size          = sizeof(AF_TYPE(coro_)),             \
    .coro          = AF_NAME(coro_),                     \
    .tick_duration = (tick_duration_),                   \
    .self          = (self_) };                                   \
  do {                                                   \
    if (offsetof(AF_TYPE(coro_), return_value) !=        \
            offsetof(laplace_promise_t, return_value) || \
        offsetof(AF_TYPE(coro_), alloc) !=               \
            offsetof(laplace_promise_t, alloc) ||        \
        offsetof(AF_TYPE(coro_), access) !=              \
            offsetof(laplace_promise_t, access) ||       \
        offsetof(AF_TYPE(coro_), self) !=                \
            offsetof(laplace_promise_t, self))           \
      abort();                                           \
  } while (0)

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define ACTION LAPLACE_ACTION

#  define promise_t laplace_promise_t
#  define action_t laplace_action_t
#  define action_starter_fn laplace_action_starter_fn
#endif

#ifdef __cplusplus
}
#endif

#endif
