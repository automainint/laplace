#include "action_dispatch.h"

#ifndef LAPLACE_ENABLE_STATIC_DISPATCH
void laplace_action_dispatch(laplace_promise_t *const promise) {
  /*  Fallback to dynamic dispatch.
   */
  assert(0);
  AF_EXECUTE(promise);
}
#endif
