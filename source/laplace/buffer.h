#ifndef LAPLACE_BUFFER_H
#define LAPLACE_BUFFER_H

#include "handle.h"

#include <kit/atomic.h>
#include <kit/condition_variable.h>
#include <kit/dynamic_array.h>
#include <kit/mutex.h>

#include <assert.h>
#include <string.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*  TODO
 *  - Implement lock-free data allocation.
 */

enum { LAPLACE_BUFFER_DEFAULT_CHUNK_SIZE = 400 };

/*  FIXME
 *  Are atomics essential here?
 */
typedef struct {
  KIT_ATOMIC(ptrdiff_t) index;
  KIT_ATOMIC(ptrdiff_t) generation;
  KIT_ATOMIC(ptrdiff_t) size;
} laplace_buf_block_t_;

typedef struct {
  int       empty;
  ptrdiff_t offset;
} laplace_buf_info_t_;

typedef struct {
  KIT_ATOMIC(int8_t) flag;
} laplace_buf_changed_t_;

#define LAPLACE_BUFFER_DATA                 \
  struct {                                  \
    ptrdiff_t cell_size;                    \
    ptrdiff_t chunk_size;                   \
    ptrdiff_t reserved;                     \
    ptrdiff_t next_block;                   \
    mtx_t     read_lock;                    \
    cnd_t     read_sync;                    \
    ptrdiff_t read_count;                   \
    KIT_ATOMIC(ptrdiff_t) next_chunk;       \
    KIT_DA(laplace_buf_changed_t_) changed; \
    KIT_DA(laplace_buf_info_t_) info;       \
    KIT_DA(laplace_buf_block_t_) blocks;    \
  }

#define LAPLACE_BUFFER_CELL(element_type_) \
  struct {                                 \
    KIT_ATOMIC(element_type_) delta;       \
    KIT_ATOMIC(element_type_) value;       \
  }

typedef struct {
  LAPLACE_BUFFER_DATA;
  KIT_DA(void) data;
} laplace_buffer_void_t;

#define LAPLACE_BUFFER_TYPE(element_type_)           \
  struct {                                           \
    LAPLACE_BUFFER_DATA;                             \
    KIT_DA(LAPLACE_BUFFER_CELL(element_type_)) data; \
  }

#define LAPLACE_BUFFER_INIT(status_, buf_, alloc_)                \
  do {                                                            \
    (status_) = KIT_OK;                                           \
    memset(&(buf_), 0, sizeof(buf_));                             \
    (buf_).cell_size  = sizeof *(buf_).data.values;               \
    (buf_).chunk_size = LAPLACE_BUFFER_DEFAULT_CHUNK_SIZE;        \
    if (mtx_init(&(buf_).read_lock, mtx_plain) != thrd_success) { \
      (status_) = LAPLACE_ERROR_BAD_MUTEX_INIT;                   \
    } else if (cnd_init(&(buf_).read_sync) != thrd_success) {     \
      (void) mtx_destroy(&(buf_).read_lock);                      \
      (status_) = LAPLACE_ERROR_BAD_CNDVAR_INIT;                  \
    } else {                                                      \
      (buf_).read_count = 0;                                      \
      KIT_DA_INIT((buf_).changed, 0, (alloc_));                   \
      KIT_DA_INIT((buf_).info, 0, (alloc_));                      \
      KIT_DA_INIT((buf_).blocks, 0, (alloc_));                    \
      KIT_DA_INIT((buf_).data, 0, (alloc_));                      \
    }                                                             \
  } while (0)

#define LAPLACE_BUFFER_DESTROY(buffer_)                              \
  do {                                                               \
    if (mtx_lock(&(buffer_).read_lock) == thrd_success) {            \
      while ((buffer_).read_count != 0 &&                            \
             cnd_wait(&(buffer_).read_sync, &(buffer_).read_lock) == \
                 thrd_success) { }                                   \
      (void) mtx_unlock(&(buffer_).read_lock);                       \
    }                                                                \
    mtx_destroy(&(buffer_).read_lock);                               \
    cnd_destroy(&(buffer_).read_sync);                               \
    KIT_DA_DESTROY((buffer_).changed);                               \
    KIT_DA_DESTROY((buffer_).info);                                  \
    KIT_DA_DESTROY((buffer_).blocks);                                \
    KIT_DA_DESTROY((buffer_).data);                                  \
  } while (0)

kit_status_t laplace_buffer_set_chunk_size(
    laplace_buffer_void_t *buffer, ptrdiff_t chunk_size);

laplace_handle_t laplace_buffer_allocate(
    laplace_buffer_void_t *buffer, ptrdiff_t size);

laplace_handle_t laplace_buffer_allocate_into(
    laplace_buffer_void_t *buffer, ptrdiff_t size,
    laplace_handle_t handle);

typedef struct {
  kit_status_t status;
  ptrdiff_t    offset;
  ptrdiff_t    previous_offset;
  ptrdiff_t    previous_size;
} laplace_buffer_realloc_result_t;

laplace_buffer_realloc_result_t laplace_buffer_reallocate(
    laplace_buffer_void_t *buffer, ptrdiff_t size,
    laplace_handle_t handle);

kit_status_t laplace_buffer_reserve(laplace_buffer_void_t *buffer,
                                    ptrdiff_t              size);

kit_status_t laplace_buffer_deallocate(laplace_buffer_void_t *buffer,
                                       laplace_handle_t       handle);

kit_status_t laplace_buffer_check(laplace_buffer_void_t const *buffer,
                                  laplace_handle_t             handle,
                                  ptrdiff_t index, ptrdiff_t size);

ptrdiff_t laplace_buffer_size(laplace_buffer_void_t *buffer,
                              laplace_handle_t       handle);

#define LAPLACE_BUFFER_SET_CHUNK_SIZE(buf_, chunk_size_)           \
  laplace_buffer_set_chunk_size((laplace_buffer_void_t *) &(buf_), \
                                (chunk_size_))

#define LAPLACE_BUF_INDEX_UNSAFE_(buf_, id_) \
  (buf_).blocks.values[id_].index

#define LAPLACE_BUF_ZERO_(buf_, handle_, offset_, size_)          \
  do {                                                            \
    if ((handle_).id != LAPLACE_ID_UNDEFINED) {                   \
      ptrdiff_t const begin_ = LAPLACE_BUF_INDEX_UNSAFE_(         \
                                   (buf_), (handle_).id) +        \
                               (offset_);                         \
      ptrdiff_t const end_ = begin_ + (size_);                    \
      for (ptrdiff_t i_ = begin_; i_ < end_; i_++) {              \
        assert(i_ >= 0 && i_ < (buf_).data.size);                 \
        atomic_store_explicit(&(buf_).changed.values[i_].flag, 0, \
                              memory_order_relaxed);              \
      }                                                           \
      for (ptrdiff_t i_ = begin_; i_ < end_; i_++) {              \
        assert(i_ >= 0 && i_ < (buf_).data.size);                 \
        atomic_store_explicit(&(buf_).data.values[i_].value, 0,   \
                              memory_order_relaxed);              \
        atomic_store_explicit(&(buf_).data.values[i_].delta, 0,   \
                              memory_order_relaxed);              \
      }                                                           \
    }                                                             \
  } while (0)

#define LAPLACE_BUFFER_ALLOCATE(return_, buf_, size_) \
  do {                                                \
    (return_) = laplace_buffer_allocate(              \
        (laplace_buffer_void_t *) &(buf_), (size_));  \
    LAPLACE_BUF_ZERO_((buf_), (return_), 0, (size_)); \
  } while (0)

#define LAPLACE_BUFFER_ALLOCATE_INTO(return_, buf_, size_, handle_) \
  do {                                                              \
    (return_) = laplace_buffer_allocate_into(                       \
        (laplace_buffer_void_t *) &(buf_), (size_), (handle_));     \
    LAPLACE_BUF_ZERO_((buf_), (return_), 0, (size_));               \
  } while (0)

#define LAPLACE_BUFFER_REALLOCATE(status_, buf_, size_, handle_)     \
  do {                                                               \
    laplace_buffer_realloc_result_t res_ =                           \
        laplace_buffer_reallocate((laplace_buffer_void_t *) &(buf_), \
                                  (size_), (handle_));               \
    if (res_.status == KIT_OK) {                                     \
      if (res_.offset != res_.previous_offset) {                     \
        for (ptrdiff_t i_ = 0;                                       \
             i_ < res_.previous_size && i_ < (size_); i_++) {        \
          assert(res_.offset + i_ >= 0 &&                            \
                 res_.offset + i_ < (buf_).data.size);               \
          assert(res_.previous_offset + i_ >= 0 &&                   \
                 res_.previous_offset + i_ < (buf_).data.size);      \
          atomic_store_explicit(                                     \
              &(buf_).changed.values[res_.offset + i_].flag,         \
              atomic_load_explicit(                                  \
                  &(buf_)                                            \
                       .changed.values[res_.previous_offset + i_]    \
                       .flag,                                        \
                  memory_order_relaxed),                             \
              memory_order_relaxed);                                 \
        }                                                            \
        for (ptrdiff_t i_ = 0;                                       \
             i_ < res_.previous_size && i_ < (size_); i_++) {        \
          assert(res_.offset + i_ >= 0 &&                            \
                 res_.offset + i_ < (buf_).data.size);               \
          assert(res_.previous_offset + i_ >= 0 &&                   \
                 res_.previous_offset + i_ < (buf_).data.size);      \
          atomic_store_explicit(                                     \
              &(buf_).data.values[res_.offset + i_].value,           \
              atomic_load_explicit(                                  \
                  &(buf_)                                            \
                       .data.values[res_.previous_offset + i_]       \
                       .value,                                       \
                  memory_order_relaxed),                             \
              memory_order_relaxed);                                 \
          atomic_store_explicit(                                     \
              &(buf_).data.values[res_.offset + i_].delta,           \
              atomic_load_explicit(                                  \
                  &(buf_)                                            \
                       .data.values[res_.previous_offset + i_]       \
                       .delta,                                       \
                  memory_order_relaxed),                             \
              memory_order_relaxed);                                 \
        }                                                            \
      }                                                              \
      LAPLACE_BUF_ZERO_((buf_), (handle_), res_.previous_size,       \
                        (size_) -res_.previous_size);                \
    }                                                                \
    (status_) = res_.status;                                         \
  } while (0)

#define LAPLACE_BUFFER_RESERVE(buf_, size_) \
  laplace_buffer_reserve((laplace_buffer_void_t *) &(buf_), (size_))

#define LAPLACE_BUFFER_DEALLOCATE(buf_, handle_)               \
  laplace_buffer_deallocate((laplace_buffer_void_t *) &(buf_), \
                            (handle_))

#define LAPLACE_BUFFER_SIZE_THREAD_SAFE(buf_, handle_) \
  laplace_buffer_size((laplace_buffer_void_t *) &(buf_), (handle_))

#define LAPLACE_BUFFER_READ_THREAD_SAFE(status_, buf_, handle_,   \
                                        index_, size_, dst_)      \
  do {                                                            \
    if (mtx_lock(&(buf_).read_lock) != thrd_success) {            \
      memset((dst_), 0, (size_) * sizeof *(dst_));                \
      (status_) = LAPLACE_ERROR_BAD_MUTEX_LOCK;                   \
    } else {                                                      \
      (buf_).read_count++;                                        \
      (void) mtx_unlock(&(buf_).read_lock);                       \
      (status_) = laplace_buffer_check(                           \
          (laplace_buffer_void_t *) &(buf_), (handle_), (index_), \
          (size_));                                               \
      if ((status_) == KIT_OK) {                                  \
        ptrdiff_t const begin_ =                                  \
            atomic_load_explicit(                                 \
                &(buf_).blocks.values[(handle_).id].index,        \
                memory_order_acquire) +                           \
            (index_);                                             \
        for (ptrdiff_t i_ = 0; i_ < (size_); i_++) {              \
          assert(begin_ >= 0 && begin_ + i_ < (buf_).data.size);  \
          (dst_)[i_] = atomic_load_explicit(                      \
              &(buf_).data.values[begin_ + i_].value,             \
              memory_order_relaxed);                              \
        }                                                         \
      } else {                                                    \
        memset((dst_), 0, (size_) * sizeof *(dst_));              \
      }                                                           \
      if (mtx_lock(&(buf_).read_lock) != thrd_success) {          \
        (status_) = LAPLACE_ERROR_BAD_MUTEX_LOCK;                 \
      } else {                                                    \
        ptrdiff_t const count_ = --(buf_).read_count;             \
        (void) mtx_unlock(&(buf_).read_lock);                     \
        if (count_ == 0 &&                                        \
            cnd_broadcast(&(buf_).read_sync) != thrd_success)     \
          (status_) = LAPLACE_ERROR_BAD_CNDVAR_BROADCAST;         \
      }                                                           \
    }                                                             \
  } while (0)

#define LAPLACE_BUFFER_SET(status_, buf_, handle_, index_, value_)   \
  do {                                                               \
    (status_) = laplace_buffer_check(                                \
        (laplace_buffer_void_t *) &(buf_), (handle_), (index_), 1);  \
    if ((status_) == KIT_OK) {                                       \
      ptrdiff_t const offset_ = LAPLACE_BUF_INDEX_UNSAFE_(           \
                                    (buf_), (handle_).id) +          \
                                (index_);                            \
      assert(offset_ >= 0 && offset_ < (buf_).data.size);            \
      atomic_store_explicit(&(buf_).changed.values[offset_].flag, 1, \
                            memory_order_relaxed);                   \
      atomic_fetch_add_explicit(                                     \
          &(buf_).data.values[offset_].delta,                        \
          (value_) -atomic_load_explicit(                            \
              &(buf_).data.values[offset_].value,                    \
              memory_order_relaxed),                                 \
          memory_order_relaxed);                                     \
    }                                                                \
  } while (0)

#define LAPLACE_BUFFER_ADD(status_, buf_, handle_, index_, delta_)   \
  do {                                                               \
    (status_) = laplace_buffer_check(                                \
        (laplace_buffer_void_t *) &(buf_), (handle_), (index_), 1);  \
    if ((status_) == KIT_OK) {                                       \
      ptrdiff_t const offset_ = LAPLACE_BUF_INDEX_UNSAFE_(           \
                                    (buf_), (handle_).id) +          \
                                (index_);                            \
      assert(offset_ >= 0 && offset_ < (buf_).data.size);            \
      atomic_store_explicit(&(buf_).changed.values[offset_].flag, 1, \
                            memory_order_relaxed);                   \
      atomic_fetch_add_explicit(&(buf_).data.values[offset_].delta,  \
                                (delta_), memory_order_relaxed);     \
    }                                                                \
  } while (0)

#define LAPLACE_BUFFER_SET_N(status_, buf_, handle_, index_, size_, \
                             values_)                               \
  do {                                                              \
    (status_) = laplace_buffer_check(                               \
        (laplace_buffer_void_t *) &(buf_), (handle_), (index_),     \
        (size_));                                                   \
    if ((status_) == KIT_OK) {                                      \
      ptrdiff_t const offset_ = LAPLACE_BUF_INDEX_UNSAFE_(          \
                                    (buf_), (handle_).id) +         \
                                (index_);                           \
      for (ptrdiff_t i_ = 0; i_ < (size_); i_++)                    \
        atomic_store_explicit(                                      \
            &(buf_).changed.values[offset_ + i_].flag, 1,           \
            memory_order_relaxed);                                  \
      for (ptrdiff_t i_ = 0; i_ < (size_); i_++)                    \
        atomic_fetch_add_explicit(                                  \
            &(buf_).data.values[offset_ + i_].delta,                \
            (values_)[i_] -                                         \
                atomic_load_explicit(                               \
                    &(buf_).data.values[offset_ + i_].value,        \
                    memory_order_relaxed),                          \
            memory_order_relaxed);                                  \
    }                                                               \
  } while (0)

#define LAPLACE_BUFFER_ADD_N(status_, buf_, handle_, index_, size_, \
                             deltas_)                               \
  do {                                                              \
    (status_) = laplace_buffer_check(                               \
        (laplace_buffer_void_t *) &(buf_), (handle_), (index_),     \
        (size_));                                                   \
    if ((status_) == KIT_OK) {                                      \
      ptrdiff_t const offset_ = LAPLACE_BUF_INDEX_UNSAFE_(          \
                                    (buf_), (handle_).id) +         \
                                (index_);                           \
      for (ptrdiff_t i_ = 0; i_ < (size_); i_++)                    \
        atomic_store_explicit(                                      \
            &(buf_).changed.values[offset_ + i_].flag, 1,           \
            memory_order_relaxed);                                  \
      for (ptrdiff_t i_ = 0; i_ < (size_); i_++)                    \
        atomic_fetch_add_explicit(                                  \
            &(buf_).data.values[offset_ + i_].delta, (deltas_)[i_], \
            memory_order_relaxed);                                  \
    }                                                               \
  } while (0)

/*  FIXME
 *  Measure performance.
 */
#define LAPLACE_BUFFER_ADJUST(return_, buffer_, element_type_)       \
  do {                                                               \
    ptrdiff_t const begin_ = atomic_fetch_add_explicit(              \
        &(buffer_).next_chunk, (buffer_).chunk_size,                 \
        memory_order_relaxed);                                       \
    ptrdiff_t end_ = begin_ + (buffer_).chunk_size;                  \
    if (end_ > (buffer_).data.size)                                  \
      end_ = (buffer_).data.size;                                    \
    assert(begin_ >= 0 && end_ <= (buffer_).data.size);              \
    assert((buffer_).changed.size == (buffer_).data.size);           \
    for (ptrdiff_t i_ = begin_; i_ < end_;) {                        \
      while (i_ < end_ && atomic_exchange_explicit(                  \
                              &(buffer_).changed.values[i_].flag, 0, \
                              memory_order_relaxed) == 0)            \
        i_++;                                                        \
      while (i_ < end_) {                                            \
        element_type_ const delta_ = atomic_exchange_explicit(       \
            &(buffer_).data.values[i_].delta, 0,                     \
            memory_order_relaxed);                                   \
        if (delta_ == 0) {                                           \
          i_++;                                                      \
          break;                                                     \
        }                                                            \
        atomic_fetch_add_explicit(&(buffer_).data.values[i_].value,  \
                                  delta_, memory_order_relaxed);     \
        i_++;                                                        \
      }                                                              \
    }                                                                \
    (return_) = (end_ != (buffer_).data.size);                       \
  } while (0)

#define LAPLACE_BUFFER_ADJUST_DONE(buffer_)       \
  atomic_store_explicit(&(buffer_).next_chunk, 0, \
                        memory_order_relaxed)

/*  FIXME
 *  Add tests for this.
 *
 *  Can clone only after join and before schedule.
 */
#define LAPLACE_BUFFER_CLONE(s, dst, src)                         \
  do {                                                            \
    kit_status_t status_ = KIT_OK;                                \
    (dst).cell_size      = (src).cell_size;                       \
    (dst).chunk_size     = (src).chunk_size;                      \
    (dst).reserved       = (src).reserved;                        \
    (dst).next_block     = (src).next_block;                      \
    atomic_store_explicit(&(dst).next_chunk, 0,                   \
                          memory_order_relaxed);                  \
    KIT_DA_RESIZE((dst).changed, (src).changed.size);             \
    if ((dst).changed.size != (src).changed.size)                 \
      status_ = LAPLACE_ERROR_BAD_ALLOC;                          \
    else                                                          \
      for (ptrdiff_t i_ = 0; i_ < (src).changed.size; i_++)       \
        atomic_store_explicit(&(dst).changed.values[i_].flag, 0,  \
                              memory_order_relaxed);              \
    KIT_DA_RESIZE((dst).info, (src).info.size);                   \
    if ((dst).info.size != (src).info.size)                       \
      status_ = LAPLACE_ERROR_BAD_ALLOC;                          \
    else if ((dst).info.size > 0)                                 \
      memcpy((dst).info.values, (src).info.values,                \
             sizeof((src).info.values[0]) * (src).info.size);     \
    KIT_DA_RESIZE((dst).blocks, (src).blocks.size);               \
    if ((dst).blocks.size != (src).blocks.size)                   \
      status_ = LAPLACE_ERROR_BAD_ALLOC;                          \
    else if ((dst).blocks.size > 0)                               \
      memcpy((dst).blocks.values, (src).blocks.values,            \
             sizeof((src).blocks.values[0]) * (src).blocks.size); \
    KIT_DA_RESIZE((dst).data, (src).data.size);                   \
    if ((dst).data.size != (src).data.size)                       \
      status_ = LAPLACE_ERROR_BAD_ALLOC;                          \
    else                                                          \
      for (ptrdiff_t i_ = 0; i_ < (src).data.size; i_++) {        \
        atomic_store_explicit(                                    \
            &(dst).data.values[i_].value,                         \
            atomic_load_explicit(&(src).data.values[i_].value,    \
                                 memory_order_relaxed),           \
            memory_order_relaxed);                                \
        atomic_store_explicit(&(dst).data.values[i_].delta, 0,    \
                              memory_order_relaxed);              \
      }                                                           \
    (s) = status_;                                                \
  } while (0)

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define BUFFER_TYPE LAPLACE_BUFFER_TYPE
#  define BUFFER_INIT LAPLACE_BUFFER_INIT
#  define BUFFER_CREATE LAPLACE_BUFFER_CREATE
#  define BUFFER_DESTROY LAPLACE_BUFFER_DESTROY
#  define BUFFER_SET_CHUNK_SIZE LAPLACE_BUFFER_SET_CHUNK_SIZE
#  define BUFFER_ALLOCATE LAPLACE_BUFFER_ALLOCATE
#  define BUFFER_ALLOCATE_INTO LAPLACE_BUFFER_ALLOCATE_INTO
#  define BUFFER_REALLOCATE LAPLACE_BUFFER_REALLOCATE
#  define BUFFER_RESERVE LAPLACE_BUFFER_RESERVE
#  define BUFFER_DEALLOCATE LAPLACE_BUFFER_DEALLOCATE
#  define BUFFER_SIZE_THREAD_SAFE LAPLACE_BUFFER_SIZE_THREAD_SAFE
#  define BUFFER_READ_THREAD_SAFE LAPLACE_BUFFER_READ_THREAD_SAFE
#  define BUFFER_SET LAPLACE_BUFFER_SET
#  define BUFFER_ADD LAPLACE_BUFFER_ADD
#  define BUFFER_ADJUST LAPLACE_BUFFER_ADJUST
#  define BUFFER_ADJUST_LOOP LAPLACE_BUFFER_ADJUST_LOOP
#  define BUFFER_ADJUST_DONE LAPLACE_BUFFER_ADJUST_DONE
#  define BUFFER_CLONE LAPLACE_BUFFER_CLONE

#  define BUFFER_DEFAULT_CHUNK_SIZE LAPLACE_BUFFER_DEFAULT_CHUNK_SIZE
#endif

#ifdef __cplusplus
}
#endif

#endif
