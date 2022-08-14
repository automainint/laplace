#include "state.h"

#include "buffer.h"

#include <kit/atomic.h>

typedef struct {
  ATOMIC(ptrdiff_t) ref_count;
  kit_allocator_t alloc;
  LAPLACE_BUFFER_TYPE(laplace_integer_t) integers;
  LAPLACE_BUFFER_TYPE(laplace_byte_t) bytes;
} state_internal_t;

static void acquire(void *p) {
  state_internal_t *internal = (state_internal_t *) p;

  atomic_fetch_add_explicit(&internal->ref_count, 1,
                            memory_order_release);
}

static void destroy(state_internal_t *internal) {
  LAPLACE_BUFFER_DESTROY(internal->integers);
  LAPLACE_BUFFER_DESTROY(internal->bytes);

  internal->alloc.deallocate(internal->alloc.state, internal);
}

static void release(void *p) {
  state_internal_t *internal = (state_internal_t *) p;

  if (atomic_fetch_add_explicit(&internal->ref_count, -1,
                                memory_order_seq_cst) == 1)
    destroy(internal);
}

#define CLONE_BUF(s, src, dst)                                       \
  do {                                                               \
    (dst).chunk_size = (src).chunk_size;                             \
    (dst).reserved   = (src).reserved;                               \
    (dst).next_block = (src).next_block;                             \
    atomic_store_explicit(                                           \
        &(dst).next_chunk,                                           \
        atomic_load_explicit(&(src).next_chunk,                      \
                             memory_order_relaxed),                  \
        memory_order_relaxed);                                       \
    ptrdiff_t n_ = (src).blocks.size;                                \
    DA_RESIZE((src).blocks, n_);                                     \
    if ((src).blocks.size != n_)                                     \
      (s) = LAPLACE_STATE_ERROR_BAD_ALLOC;                           \
    else                                                             \
      memcpy((src).blocks.values, (src).blocks.values,               \
             sizeof((src).blocks.values[0]) * n_);                   \
    n_ = (src).data.size;                                            \
    DA_RESIZE((src).data, n_);                                       \
    if ((src).data.size != n_)                                       \
      (s) = LAPLACE_STATE_ERROR_BAD_ALLOC;                           \
    else {                                                           \
      for (ptrdiff_t i_ = 0; i_ < n_; i_++) {                        \
        (dst).data.values[i_].empty  = (src).data.values[i_].empty;  \
        (dst).data.values[i_].offset = (src).data.values[i_].offset; \
        atomic_store_explicit(                                       \
            &(dst).data.values[i_].value,                            \
            atomic_load_explicit(&(src).data.values[i_].value,       \
                                 memory_order_relaxed),              \
            memory_order_relaxed);                                   \
        atomic_store_explicit(&(dst).data.values[i_].delta, 0,       \
                              memory_order_relaxed);                 \
      }                                                              \
    }                                                                \
  } while (0)

static laplace_status_t clone(void *p, laplace_read_write_t *cloned) {
  state_internal_t *self = (state_internal_t *) p;

  laplace_status_t s = laplace_state_init(cloned, self->alloc);
  if (s != LAPLACE_STATUS_OK)
    return s;

  state_internal_t *clone_self = (state_internal_t *) cloned->state;

  CLONE_BUF(s, self->integers, clone_self->integers);
  CLONE_BUF(s, self->bytes, clone_self->bytes);

  if (s != LAPLACE_STATUS_OK) {
    destroy(clone_self);
    return s;
  }

  return LAPLACE_STATUS_OK;
}

static laplace_status_t read_integers(
    void *p, laplace_handle_t handle, ptrdiff_t index, ptrdiff_t size,
    laplace_integer_t *destination) {
  state_internal_t *internal = (state_internal_t *) p;

  laplace_status_t s = laplace_buffer_check(
      (laplace_buffer_void_t *) &internal->integers,
      sizeof internal->integers.data.values[0], handle, index, size);

  if (s != LAPLACE_STATUS_OK)
    return s;

  for (ptrdiff_t i = 0; i < size; i++)
    destination[i] = LAPLACE_BUFFER_GET_UNSAFE(internal->integers,
                                               handle, index + i);
}

static laplace_status_t read_bytes(void *p, laplace_handle_t handle,
                                   ptrdiff_t index, ptrdiff_t size,
                                   laplace_byte_t *destination) {
  state_internal_t *internal = (state_internal_t *) p;

  laplace_status_t s = laplace_buffer_check(
      (laplace_buffer_void_t *) &internal->bytes,
      sizeof internal->bytes.data.values[0], handle, index, size);

  if (s != LAPLACE_STATUS_OK)
    return s;

  for (ptrdiff_t i = 0; i < size; i++)
    destination[i] = LAPLACE_BUFFER_GET_UNSAFE(internal->bytes,
                                               handle, index + i);
}

static laplace_integer_t get_integer(void *p, laplace_handle_t handle,
                                     ptrdiff_t         index,
                                     laplace_integer_t invalid) {
  state_internal_t *internal = (state_internal_t *) p;
  return LAPLACE_BUFFER_GET(internal->integers, handle, index,
                            invalid);
}

static laplace_byte_t get_byte(void *p, laplace_handle_t handle,
                               ptrdiff_t      index,
                               laplace_byte_t invalid) {
  state_internal_t *internal = (state_internal_t *) p;
  return LAPLACE_BUFFER_GET(internal->bytes, handle, index, invalid);
}

static laplace_status_t apply(void                   *p,
                              laplace_impact_t const *impact) {
  state_internal_t *internal = (state_internal_t *) p;
  return LAPLACE_STATUS_OK;
}

static void adjust_loop(void *p) {
  state_internal_t *internal = (state_internal_t *) p;

  for (int ints = 1, bytes = 1; ints || bytes;) {
    if (ints)
      LAPLACE_BUFFER_ADJUST(ints, internal->integers);
    if (bytes)
      LAPLACE_BUFFER_ADJUST(bytes, internal->bytes);
  }
}

static void adjust_done(void *p) {
  state_internal_t *internal = (state_internal_t *) p;

  LAPLACE_BUFFER_ADJUST_DONE(internal->integers);
  LAPLACE_BUFFER_ADJUST_DONE(internal->bytes);
}

laplace_status_t laplace_state_init(laplace_read_write_t *const p,
                                    kit_allocator_t const alloc) {
  state_internal_t *internal = (state_internal_t *) alloc.allocate(
      alloc.state, sizeof(state_internal_t));

  if (internal == NULL)
    return LAPLACE_STATE_ERROR_BAD_ALLOC;

  atomic_store_explicit(&internal->ref_count, 0,
                        memory_order_relaxed);
  internal->alloc = alloc;

  LAPLACE_BUFFER_INIT(internal->integers, alloc);
  LAPLACE_BUFFER_INIT(internal->bytes, alloc);

  p->state         = internal;
  p->acquire       = acquire;
  p->release       = release;
  p->clone         = clone;
  p->read_integers = read_integers;
  p->read_bytes    = read_bytes;
  p->get_integer   = get_integer;
  p->get_byte      = get_byte;
  p->apply         = apply;
  p->adjust_loop   = adjust_loop;
  p->adjust_done   = adjust_done;

  return LAPLACE_STATUS_OK;
}
