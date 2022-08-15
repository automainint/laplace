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

typedef laplace_status_t (*laplace_action_starter_fn)(
    laplace_promise_t *coroutine, kit_allocator_t alloc,
    laplace_handle_t self, laplace_read_only_t access);

typedef struct {
  ptrdiff_t                 size;
  laplace_action_starter_fn starter;
  laplace_time_t            tick_duration;
  laplace_handle_t          self;
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
