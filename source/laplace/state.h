#ifndef LAPLACE_STATE_H
#define LAPLACE_STATE_H

#include "access.h"

#include <kit/allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

kit_status_t laplace_state_init(laplace_read_write_t *state,
                                uint64_t seed, kit_allocator_t alloc);

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define state_init laplace_state_init
#endif

#ifdef __cplusplus
}
#endif

#endif
