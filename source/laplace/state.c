#include "state.h"

#include "buffer.h"
#include "impact.h"
#include "rng.h"

#include <kit/atomic.h>

typedef struct {
  ATOMIC(ptrdiff_t) ref_count;
  kit_allocator_t alloc;
  LAPLACE_BUFFER_TYPE(laplace_integer_t) integers;
  LAPLACE_BUFFER_TYPE(laplace_byte_t) bytes;
  laplace_rng_state_t random_state;
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

static ptrdiff_t integers_size(void *p, laplace_handle_t handle) {
  state_internal_t *internal = (state_internal_t *) p;

  if (laplace_buffer_check(
          (laplace_buffer_void_t *) &internal->integers,
          sizeof internal->integers.data.values[0], handle, 0,
          0) != LAPLACE_STATUS_OK)
    return 0;

  return internal->integers.blocks.values[handle.id].size;
}

static ptrdiff_t bytes_size(void *p, laplace_handle_t handle) {
  state_internal_t *internal = (state_internal_t *) p;

  if (laplace_buffer_check((laplace_buffer_void_t *) &internal->bytes,
                           sizeof internal->bytes.data.values[0],
                           handle, 0, 0) != LAPLACE_STATUS_OK)
    return 0;

  return internal->bytes.blocks.values[handle.id].size;
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

DA_TYPE(integers_t, laplace_integer_t);
DA_TYPE(bytes_t, laplace_byte_t);

static laplace_status_t apply(void *const                   p,
                              laplace_impact_t const *const impact) {
  state_internal_t *internal = (state_internal_t *) p;
  laplace_status_t  s        = LAPLACE_STATUS_OK;

#define CASES_T(TYPE_CAPS_, type_)                                  \
  case LAPLACE_IMPACT_##TYPE_CAPS_##_SET:                           \
    LAPLACE_BUFFER_SET(                                             \
        s, internal->type_##s, impact->type_##_set.handle,          \
        impact->type_##_set.index, impact->type_##_set.value);      \
    break;                                                          \
  case LAPLACE_IMPACT_##TYPE_CAPS_##_ADD:                           \
    LAPLACE_BUFFER_ADD(                                             \
        s, internal->type_##s, impact->type_##_add.handle,          \
        impact->type_##_add.index, impact->type_##_add.delta);      \
    break;                                                          \
  case LAPLACE_IMPACT_##TYPE_CAPS_##_WRITE_VALUES:                  \
    LAPLACE_BUFFER_SET_N(                                           \
        s, internal->type_##s, impact->type_##_write_values.handle, \
        impact->type_##_write_values.index,                         \
        impact->type_##_write_values.values.size,                   \
        impact->type_##_write_values.values.values);                \
    break;                                                          \
  case LAPLACE_IMPACT_##TYPE_CAPS_##_WRITE_DELTAS:                  \
    LAPLACE_BUFFER_ADD_N(                                           \
        s, internal->type_##s, impact->type_##_write_deltas.handle, \
        impact->type_##_write_deltas.index,                         \
        impact->type_##_write_deltas.deltas.size,                   \
        impact->type_##_write_deltas.deltas.values);                \
    break;                                                          \
  case LAPLACE_IMPACT_##TYPE_CAPS_##_RESERVE:                       \
    s = LAPLACE_BUFFER_RESERVE(internal->type_##s,                  \
                               impact->type_##_reserve.count);      \
    break;                                                          \
  case LAPLACE_IMPACT_##TYPE_CAPS_##_ALLOCATE_INTO: {               \
    laplace_handle_t h;                                             \
    LAPLACE_BUFFER_ALLOCATE_INTO(                                   \
        h, internal->type_##s, impact->type_##_allocate_into.size,  \
        impact->type_##_allocate_into.handle);                      \
    if (h.id == LAPLACE_ID_UNDEFINED)                               \
      s = h.error;                                                  \
  } break;                                                          \
  case LAPLACE_IMPACT_##TYPE_CAPS_##_ALLOCATE: {                    \
    laplace_handle_t h;                                             \
    LAPLACE_BUFFER_ALLOCATE(h, internal->type_##s,                  \
                            impact->type_##_allocate.size);         \
    if (h.id != LAPLACE_ID_UNDEFINED) {                             \
      LAPLACE_BUFFER_SET(s, internal->integers,                     \
                         impact->type_##_allocate.return_handle,    \
                         impact->type_##_allocate.return_index,     \
                         h.id);                                     \
      LAPLACE_BUFFER_SET(s, internal->integers,                     \
                         impact->type_##_allocate.return_handle,    \
                         impact->type_##_allocate.return_index + 1, \
                         h.generation);                             \
    } else                                                          \
      s = h.error;                                                  \
  } break;                                                          \
  case LAPLACE_IMPACT_##TYPE_CAPS_##_REALLOCATE:                    \
    LAPLACE_BUFFER_REALLOCATE(s, internal->type_##s,                \
                              impact->type_##_reallocate.size,      \
                              impact->type_##_reallocate.handle);   \
    break;                                                          \
  case LAPLACE_IMPACT_##TYPE_CAPS_##_DEALLOCATE:                    \
    s = LAPLACE_BUFFER_DEALLOCATE(                                  \
        internal->type_##s, impact->type_##_deallocate.handle);     \
    break

  switch (impact->type) {
    CASES_T(INTEGER, integer);
    CASES_T(BYTE, byte);
    case LAPLACE_IMPACT_INTEGER_SEED:
      laplace_rng_init(&internal->random_state,
                       impact->integer_seed.seed);
      break;
    case LAPLACE_IMPACT_INTEGER_RANDOM: {
      DA(nums, laplace_integer_t);
      DA_INIT(nums, impact->integer_random.return_size,
              internal->alloc);
      for (ptrdiff_t i = 0; i < nums.size; i++)
        nums.values[i] = laplace_rng(&internal->random_state,
                                     impact->integer_random.min,
                                     impact->integer_random.max);
      LAPLACE_BUFFER_SET_N(s, internal->integers,
                           impact->integer_random.return_handle,
                           impact->integer_random.return_index,
                           nums.size, nums.values);
      DA_DESTROY(nums);
    } break;
    case LAPLACE_IMPACT_BYTE_RANDOM: {
      DA(nums, laplace_byte_t);
      DA_INIT(nums, impact->byte_random.return_size, internal->alloc);
      for (ptrdiff_t i = 0; i < nums.size; i++)
        nums.values[i] = (laplace_byte_t) laplace_rng(
            &internal->random_state, impact->byte_random.min,
            impact->byte_random.max);
      LAPLACE_BUFFER_SET_N(
          s, internal->bytes, impact->byte_random.return_handle,
          impact->byte_random.return_index, nums.size, nums.values);
      DA_DESTROY(nums);
    } break;
    case LAPLACE_IMPACT_NOOP: break;
    default: s = LAPLACE_STATE_ERROR_WRONG_IMPACT;
  }

#undef CASES_T

  return s;
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
  laplace_rng_init(&internal->random_state, laplace_rng_seed());

  LAPLACE_BUFFER_INIT(internal->integers, alloc);
  LAPLACE_BUFFER_INIT(internal->bytes, alloc);

  p->state         = internal;
  p->acquire       = acquire;
  p->release       = release;
  p->clone         = clone;
  p->integers_size = integers_size;
  p->bytes_size    = bytes_size;
  p->read_integers = read_integers;
  p->read_bytes    = read_bytes;
  p->get_integer   = get_integer;
  p->get_byte      = get_byte;
  p->apply         = apply;
  p->adjust_loop   = adjust_loop;
  p->adjust_done   = adjust_done;

  return LAPLACE_STATUS_OK;
}
