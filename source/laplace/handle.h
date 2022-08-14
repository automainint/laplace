#ifndef LAPLACE_HANDLE_H
#define LAPLACE_HANDLE_H

#include "options.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  ptrdiff_t id;
  union {
    laplace_status_t error;
    ptrdiff_t        generation;
  };
} laplace_handle_t;

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define handle_t laplace_handle_t
#endif

#ifdef __cplusplus
}
#endif

#endif
