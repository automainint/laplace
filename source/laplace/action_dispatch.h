#ifndef LAPLACE_ACTION_DISPATCH_H
#define LAPLACE_ACTION_DISPATCH_H

#include "promise.h"

/*  Application should implement this function for static action
 *  dispatch.
 *
 *  See LAPLACE_ENABLE_STATIC_DISPATCH macro.
 */
void laplace_action_dispatch(laplace_promise_t *promise);

#endif

