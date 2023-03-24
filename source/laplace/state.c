#include "state.h"

#include "buffer.h"
#include "impact.h"

#include <kit/atomic.h>
#include <kit/mersenne_twister_64.h>

typedef struct {
  ATOMIC(ptrdiff_t) ref_count;
  kit_allocator_t  alloc;
  uint64_t         seed;
  kit_mt64_state_t mt64;
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

  kit_alloc_dispatch(internal->alloc, KIT_DEALLOCATE, 0, 0, internal);
}

static void release(void *p) {
  state_internal_t *internal = (state_internal_t *) p;

  if (atomic_fetch_add_explicit(&internal->ref_count, -1,
                                memory_order_seq_cst) == 1)
    destroy(internal);
}

static kit_status_t clone(void *p, laplace_read_write_t *cloned) {
  state_internal_t *self = (state_internal_t *) p;

  kit_status_t s = laplace_state_init(cloned, self->seed,
                                      self->alloc);
  if (s != KIT_OK)
    return s;

  state_internal_t *clone_self = (state_internal_t *) cloned->state;

  LAPLACE_BUFFER_CLONE(s, clone_self->integers, self->integers);

  if (s != KIT_OK) {
    destroy(clone_self);
    return s;
  }

  LAPLACE_BUFFER_CLONE(s, clone_self->bytes, self->bytes);

  if (s != KIT_OK) {
    destroy(clone_self);
    return s;
  }

  return KIT_OK;
}

static kit_status_t reset(void *p) {
  state_internal_t *self = (state_internal_t *) p;

  mt64_init(&self->mt64, self->seed);

  LAPLACE_BUFFER_DESTROY(self->integers);
  LAPLACE_BUFFER_DESTROY(self->bytes);

  memset(&self->integers, 0, sizeof self->integers);
  memset(&self->bytes, 0, sizeof self->bytes);

  kit_status_t s;

  LAPLACE_BUFFER_INIT(s, self->integers, self->alloc);

  if (s != KIT_OK)
    return s;

  LAPLACE_BUFFER_INIT(s, self->bytes, self->alloc);

  if (s != KIT_OK)
    return s;

  return KIT_OK;
}

static ptrdiff_t integers_size(void *p, laplace_handle_t handle) {
  state_internal_t *internal = (state_internal_t *) p;

  if (laplace_buffer_check(
          (laplace_buffer_void_t *) &internal->integers, handle, 0,
          0) != KIT_OK)
    return 0;

  return internal->integers.blocks.values[handle.id].size;
}

static ptrdiff_t bytes_size(void *p, laplace_handle_t handle) {
  state_internal_t *internal = (state_internal_t *) p;

  if (laplace_buffer_check((laplace_buffer_void_t *) &internal->bytes,
                           handle, 0, 0) != KIT_OK)
    return 0;

  return internal->bytes.blocks.values[handle.id].size;
}

static kit_status_t read_integers(
    void *p, laplace_handle_t handle, ptrdiff_t index, ptrdiff_t size,
    laplace_integer_t *restrict destination) {
  state_internal_t *internal = (state_internal_t *) p;

  kit_status_t s;

  LAPLACE_BUFFER_READ_THREAD_SAFE(s, internal->integers, handle,
                                  index, size, destination);

  return s;
}

static kit_status_t read_bytes(void *p, laplace_handle_t handle,
                               ptrdiff_t index, ptrdiff_t size,
                               laplace_byte_t *restrict destination) {
  state_internal_t *internal = (state_internal_t *) p;

  kit_status_t s;

  LAPLACE_BUFFER_READ_THREAD_SAFE(s, internal->bytes, handle, index,
                                  size, destination);

  return s;
}

static laplace_integer_t get_integer(void *p, laplace_handle_t handle,
                                     ptrdiff_t         index,
                                     laplace_integer_t invalid) {
  state_internal_t *internal = (state_internal_t *) p;

  kit_status_t      s;
  laplace_integer_t x;

  LAPLACE_BUFFER_READ_THREAD_SAFE(s, internal->integers, handle,
                                  index, 1, &x);

  if (s != KIT_OK)
    return invalid;

  return x;
}

static laplace_byte_t get_byte(void *p, laplace_handle_t handle,
                               ptrdiff_t      index,
                               laplace_byte_t invalid) {
  state_internal_t *internal = (state_internal_t *) p;

  kit_status_t   s;
  laplace_byte_t x;

  LAPLACE_BUFFER_READ_THREAD_SAFE(s, internal->bytes, handle, index,
                                  1, &x);

  if (s != KIT_OK)
    return invalid;

  return x;
}

static int64_t rng(mt64_state_t *const mt64, int64_t const min,
                   int64_t const max) {
  uint64_t x = mt64_generate(mt64);
  uint64_t n = (uint64_t) (max - min + 1);
  return min + (int64_t) (x % n);
}

static kit_status_t apply(void *const                   p,
                          laplace_impact_t const *const impact) {
  state_internal_t *internal = (state_internal_t *) p;
  kit_status_t      s        = KIT_OK;

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
      mt64_init(&internal->mt64, impact->integer_seed.seed);
      break;
    case LAPLACE_IMPACT_INTEGER_RANDOM: {
      laplace_integers_t nums;
      DA_INIT(nums, impact->integer_random.return_size,
              internal->alloc);
      for (ptrdiff_t i = 0; i < nums.size; i++)
        nums.values[i] = rng(&internal->mt64,
                             impact->integer_random.min,
                             impact->integer_random.max);
      LAPLACE_BUFFER_SET_N(s, internal->integers,
                           impact->integer_random.return_handle,
                           impact->integer_random.return_index,
                           nums.size, nums.values);
      DA_DESTROY(nums);
    } break;
    case LAPLACE_IMPACT_BYTE_RANDOM: {
      laplace_bytes_t nums;
      DA_INIT(nums, impact->byte_random.return_size, internal->alloc);
      for (ptrdiff_t i = 0; i < nums.size; i++)
        nums.values[i] = (laplace_byte_t) rng(
            &internal->mt64, impact->byte_random.min,
            impact->byte_random.max);
      LAPLACE_BUFFER_SET_N(
          s, internal->bytes, impact->byte_random.return_handle,
          impact->byte_random.return_index, nums.size, nums.values);
      DA_DESTROY(nums);
    } break;
    case LAPLACE_IMPACT_NOOP: break;
    default: s = LAPLACE_ERROR_WRONG_IMPACT;
  }

#undef CASES_T

  return s;
}

static void adjust_loop(void *p) {
  state_internal_t *internal = (state_internal_t *) p;

  for (int ints = 1, bytes = 1; ints || bytes;) {
    if (ints)
      LAPLACE_BUFFER_ADJUST(ints, internal->integers,
                            laplace_integer_t);
    if (bytes)
      LAPLACE_BUFFER_ADJUST(bytes, internal->bytes, laplace_byte_t);
  }
}

static void adjust_done(void *p) {
  state_internal_t *internal = (state_internal_t *) p;

  LAPLACE_BUFFER_ADJUST_DONE(internal->integers);
  LAPLACE_BUFFER_ADJUST_DONE(internal->bytes);
}

kit_status_t laplace_state_init(laplace_read_write_t *const p,
                                uint64_t const              seed,
                                kit_allocator_t const       alloc) {
  state_internal_t *internal = (state_internal_t *)
      kit_alloc_dispatch(alloc, KIT_ALLOCATE,
                         sizeof(state_internal_t), 0, NULL);

  if (internal == NULL)
    return LAPLACE_ERROR_BAD_ALLOC;

  atomic_store_explicit(&internal->ref_count, 0,
                        memory_order_relaxed);
  internal->alloc = alloc;
  internal->seed  = seed;
  mt64_init(&internal->mt64, seed);

  kit_status_t s;

  LAPLACE_BUFFER_INIT(s, internal->integers, alloc);

  if (s != KIT_OK)
    return s;

  LAPLACE_BUFFER_INIT(s, internal->bytes, alloc);

  if (s != KIT_OK) {
    LAPLACE_BUFFER_DESTROY(internal->integers);
    return s;
  }

  p->state         = internal;
  p->acquire       = acquire;
  p->release       = release;
  p->clone         = clone;
  p->reset         = reset;
  p->integers_size = integers_size;
  p->bytes_size    = bytes_size;
  p->read_integers = read_integers;
  p->read_bytes    = read_bytes;
  p->get_integer   = get_integer;
  p->get_byte      = get_byte;
  p->apply         = apply;
  p->adjust_loop   = adjust_loop;
  p->adjust_done   = adjust_done;

  return KIT_OK;
}
