#ifndef LAPLACE_ASYNC_FUNCTION_H
#define LAPLACE_ASYNC_FUNCTION_H

#include "action_dispatch.h"
#include "promise.h"

/*  Redefine async function dispatch.
 */

#undef KIT_AF_AWAIT
#undef KIT_AF_YIELD_AWAIT

#define KIT_AF_AWAIT(promise_)                \
  do {                                        \
    case KIT_AF_LINE_():                      \
      if ((promise_)._index != -1) {          \
        self->_index = KIT_AF_LINE_();        \
        laplace_action_dispatch(&(promise_)); \
      }                                       \
      if ((promise_)._index != -1)            \
        return;                               \
  } while (0)

#define KIT_AF_YIELD_AWAIT(promise_)                  \
  do {                                                \
    case KIT_AF_LINE_():                              \
      if ((promise_)._index != -1) {                  \
        self->_index = KIT_AF_LINE_();                \
        laplace_action_dispatch(&(promise_));         \
        self->return_value = (promise_).return_value; \
        return;                                       \
      }                                               \
  } while (0)

#endif
