#ifndef LAPLACE_ACTION_H
#define LAPLACE_ACTION_H

#include "access.h"
#include "handle.h"
#include <kit/allocator.h>
#include <kit/async_function.h>

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

#define LAPLACE_ACTION(coro_, tick_duration_, self_)   \
  {                                                    \
    .size          = sizeof(AF_TYPE(test_impact_gen)), \
    .coro          = AF_NAME(test_impact_gen),         \
    .tick_duration = (tick_duration_), .self = (self_) \
  }

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
