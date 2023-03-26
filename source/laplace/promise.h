#ifndef LAPLACE_PROMISE_H
#define LAPLACE_PROMISE_H

#include "access.h"
#include "async_function.h"
#include "handle.h"
#include "impact.h"
#include <kit/allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

struct laplace_promise {
  KIT_AF_STATE_DATA;
  laplace_impact_list_t return_value;
  ptrdiff_t             action_id;
  kit_allocator_t       alloc;
  laplace_read_only_t   access;
  laplace_handle_t      self;
};

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define promise_t laplace_promise_t
#endif

#ifdef __cplusplus
}
#endif
#endif