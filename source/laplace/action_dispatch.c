#include "action_dispatch.h"

#include <assert.h>

#ifndef LAPLACE_ENABLE_STATIC_DISPATCH
void laplace_action_dispatch(laplace_promise_t *const promise) {
  /*  Fallback to dynamic dispatch.
   */
  assert(0);
  promise->_state_machine(promise_, kit_af_request_execute);
}
#endif
