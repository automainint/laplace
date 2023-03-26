#ifndef LAPLACE_ACTION_DISPATCH_H
#define LAPLACE_ACTION_DISPATCH_H

typedef struct laplace_promise laplace_promise_t;

/*  Application should implement this function for static action
 *  dispatch.
 *
 *  See LAPLACE_ENABLE_STATIC_DISPATCH macro.
 */
void laplace_action_dispatch(laplace_promise_t *promise);

#endif

