#ifndef LAPLACE_STATE_H
#define LAPLACE_STATE_H

#include "access.h"

#include <kit/allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

void laplace_state_init(laplace_read_write_t *state,
                        kit_allocator_t       alloc);

laplace_read_write_t laplace_state_create(void);

#ifdef __cplusplus
}
#endif

#endif
