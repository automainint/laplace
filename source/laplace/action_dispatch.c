#include "generator.h"

#ifndef LAPLACE_ENABLE_STATIC_DISPATCH
void laplace_action_dispatch(ptrdiff_t const          action_id,
                             laplace_promise_t *const promise) {
  /*  Fallback to dynamic dispatch.
   */
  assert(0);
  AF_EXECUTE(promise);
}
#endif
