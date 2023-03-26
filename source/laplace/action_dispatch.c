#include "action_dispatch.h"

#include "promise.h"
#include <assert.h>

#ifndef LAPLACE_ENABLE_STATIC_DISPATCH
void laplace_action_dispatch(laplace_promise_t *const promise) {
  /*  Fallback to dynamic dispatch.
   */
  promise->_state_machine(promise, AF_REQUEST_EXECUTE);
}
#endif
