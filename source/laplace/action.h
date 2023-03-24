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
  ptrdiff_t id; /* Unique Id for each coroutine function. Only
                   required for static action dispatch. */
  ptrdiff_t            size;
  kit_af_state_machine coro;
  laplace_time_t       tick_duration;
  laplace_handle_t     self;
} laplace_action_t;

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define promise_t laplace_promise_t
#  define action_t laplace_action_t
#  define action_starter_fn laplace_action_starter_fn
#endif

#ifdef __cplusplus
}
#endif

#endif
