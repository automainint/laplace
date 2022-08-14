#include "state.h"

#include "buffer.h"

#include <kit/atomic.h>

typedef struct {
  ATOMIC(ptrdiff_t) ref_count;
  kit_allocator_t alloc;
  LAPLACE_BUFFER_TYPE(laplace_integer_t) integers;
  LAPLACE_BUFFER_TYPE(laplace_byte_t) bytes;
} state_internal_t;

static void acquire(void *state) {
  state_internal_t *internal = (state_internal_t *) state;
  atomic_fetch_add_explicit(&internal->ref_count, 1,
                            memory_order_relaxed);
}

static void release(void *state) {
  state_internal_t *internal = (state_internal_t *) state;
  if (atomic_fetch_add_explicit(&internal->ref_count, -1,
                                memory_order_relaxed) == 1)
    internal->alloc.deallocate(internal->alloc.state, internal);
}

static laplace_read_write_t clone(void *state) {
  state_internal_t *internal = (state_internal_t *) state;
}

static laplace_status_t read_integers(
    void *state, laplace_handle_t handle, ptrdiff_t index,
    ptrdiff_t size, laplace_integer_t *destination) {
  state_internal_t *internal = (state_internal_t *) state;
  laplace_status_t  s        = laplace_buffer_check(
      (laplace_buffer_void_t *) &internal->integers,
      sizeof internal->integers.data.values[0], handle, index, size);
  if (s != LAPLACE_STATUS_OK)
    return s;
  for (ptrdiff_t i = 0; i < size; i++)
    destination[i] = LAPLACE_BUFFER_GET_UNSAFE(internal->integers,
                                               handle, index + i);
}

static laplace_status_t read_bytes_fn(void            *state,
                                      laplace_handle_t handle,
                                      ptrdiff_t index, ptrdiff_t size,
                                      laplace_byte_t *destination) {
  state_internal_t *internal = (state_internal_t *) state;
}

static laplace_integer_t get_integer(void             *state,
                                     laplace_handle_t  handle,
                                     ptrdiff_t         index,
                                     laplace_integer_t invalid) {
  state_internal_t *internal = (state_internal_t *) state;
}

static laplace_byte_t get_byte(void *state, laplace_handle_t handle,
                               ptrdiff_t      index,
                               laplace_byte_t invalid) {
  state_internal_t *internal = (state_internal_t *) state;
}

static laplace_status_t apply(void                   *state,
                              laplace_impact_t const *impact) {
  state_internal_t *internal = (state_internal_t *) state;
}

static void adjust_loop(void *state) {
  state_internal_t *internal = (state_internal_t *) state;
}

static void adjust_done(void *state) {
  state_internal_t *internal = (state_internal_t *) state;
}

void laplace_state_init(laplace_read_write_t *const state,
                        kit_allocator_t const       alloc) {
  state_internal_t *internal = (state_internal_t *) alloc.allocate(
      alloc.state, sizeof(state_internal_t));

  state->state = internal;
}

laplace_read_write_t laplace_state_create(void) {
  laplace_read_write_t state;
  laplace_state_init(&state, kit_alloc_default());
  return state;
}
