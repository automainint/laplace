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

#define LAPLACE_HANDLE_NULL \
  { .id = LAPLACE_ID_UNDEFINED, .error = LAPLACE_STATUS_OK }

extern laplace_handle_t const laplace_handle_null;

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define HANDLE_NULL LAPLACE_HANDLE_NULL
#  define handle_t laplace_handle_t
#  define handle_null laplace_handle_null
#endif

#ifdef __cplusplus
}
#endif

#endif
