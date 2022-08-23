#ifndef LAPLACE_BUFFER_H
#define LAPLACE_BUFFER_H

#include "handle.h"

#include <kit/atomic.h>
#include <kit/dynamic_array.h>
#include <kit/threads.h>

#include <assert.h>
#include <string.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { LAPLACE_BUFFER_DEFAULT_CHUNK_SIZE = 8000 };

#define LAPLACE_BUF_INFO_(buf_) (buf_).info
#define LAPLACE_BUF_DATA_(buf_) (buf_).data

#define LAPLACE_BUF_CELL_SIZE_(buf_) \
  sizeof(LAPLACE_BUF_DATA_(buf_).values[0])

/*  FIXME
 *  Are atomics essential here?
 */
#define LAPLACE_BUF_BLOCK_            \
  struct {                            \
    KIT_ATOMIC(ptrdiff_t) index;      \
    KIT_ATOMIC(ptrdiff_t) generation; \
    KIT_ATOMIC(ptrdiff_t) size;       \
  }

typedef struct {
  int       empty;
  ptrdiff_t offset;
} laplace_buf_info_t_;

#define LAPLACE_BUFFER_DATA             \
  struct {                              \
    ptrdiff_t cell_size;                \
    ptrdiff_t chunk_size;               \
    ptrdiff_t reserved;                 \
    ptrdiff_t next_block;               \
    mtx_t     read_lock;                \
    ptrdiff_t read_count;               \
    KIT_ATOMIC(ptrdiff_t) next_chunk;   \
    KIT_DA(info, laplace_buf_info_t_);  \
    KIT_DA(blocks, LAPLACE_BUF_BLOCK_); \
  }

#define LAPLACE_BUFFER_CELL(element_type_) \
  struct {                                 \
    KIT_ATOMIC(element_type_) delta;       \
    KIT_ATOMIC(element_type_) value;       \
  }

typedef struct {
  LAPLACE_BUFFER_DATA;
  KIT_DA(data, void);
} laplace_buffer_void_t;

#define LAPLACE_BUFFER_TYPE(element_type_)            \
  struct {                                            \
    LAPLACE_BUFFER_DATA;                              \
    KIT_DA(data, LAPLACE_BUFFER_CELL(element_type_)); \
  }

#define LAPLACE_BUFFER_INIT(status_, buf_, alloc_)                \
  do {                                                            \
    (status_) = LAPLACE_STATUS_OK;                                \
    memset(&(buf_), 0, sizeof(buf_));                             \
    (buf_).cell_size = LAPLACE_BUF_CELL_SIZE_(buf_);              \
    (buf_).chunk_size =                                           \
        LAPLACE_BUFFER_DEFAULT_CHUNK_SIZE /                       \
        sizeof(LAPLACE_BUF_DATA_(buf_).values[0].value);          \
    if (mtx_init(&(buf_).read_lock, mtx_plain) != thrd_success) { \
      (status_) = LAPLACE_ERROR_BAD_MUTEX_INIT;                   \
    } else {                                                      \
      (buf_).read_count = 0;                                      \
      KIT_DA_INIT((buf_).info, 0, (alloc_));                      \
      KIT_DA_INIT((buf_).blocks, 0, (alloc_));                    \
      KIT_DA_INIT((buf_).data, 0, (alloc_));                      \
    }                                                             \
  } while (0)

#define LAPLACE_BUFFER_CREATE(status_, name_, element_type_) \
  LAPLACE_BUFFER_TYPE(element_type_) name_;                  \
  LAPLACE_BUFFER_INIT(status_, name_, kit_alloc_default())

#define LAPLACE_BUFFER_DESTROY(buffer_)                     \
  do {                                                      \
    for (int done_ = 0; !done_;) {                          \
      if (mtx_lock(&(buffer_).read_lock) == thrd_success) { \
        if ((buffer_).read_count == 0)                      \
          done_ = 1;                                        \
        (void) mtx_unlock(&(buffer_).read_lock);            \
      } else                                                \
        done_ = 1;                                          \
    }                                                       \
    mtx_destroy(&(buffer_).read_lock);                      \
    KIT_DA_DESTROY((buffer_).info);                         \
    KIT_DA_DESTROY((buffer_).blocks);                       \
    KIT_DA_DESTROY((buffer_).data);                         \
  } while (0)

laplace_status_t laplace_buffer_set_chunk_size(
    laplace_buffer_void_t *buffer, ptrdiff_t chunk_size);

laplace_handle_t laplace_buffer_allocate(
    laplace_buffer_void_t *buffer, ptrdiff_t size);

laplace_handle_t laplace_buffer_allocate_into(
    laplace_buffer_void_t *buffer, ptrdiff_t size,
    laplace_handle_t handle);

typedef struct {
  laplace_status_t status;
  ptrdiff_t        offset;
  ptrdiff_t        previous_offset;
  ptrdiff_t        previous_size;
} laplace_buffer_realloc_result_t;

laplace_buffer_realloc_result_t laplace_buffer_reallocate(
    laplace_buffer_void_t *buffer, ptrdiff_t size,
    laplace_handle_t handle);

laplace_status_t laplace_buffer_reserve(laplace_buffer_void_t *buffer,
                                        ptrdiff_t              size);

laplace_status_t laplace_buffer_deallocate(
    laplace_buffer_void_t *buffer, laplace_handle_t handle);

laplace_status_t laplace_buffer_check(
    laplace_buffer_void_t const *buffer, laplace_handle_t handle,
    ptrdiff_t index, ptrdiff_t size);

ptrdiff_t laplace_buffer_size(laplace_buffer_void_t *buffer,
                              laplace_handle_t       handle);

#define LAPLACE_BUFFER_SET_CHUNK_SIZE(buf_, chunk_size_)           \
  laplace_buffer_set_chunk_size((laplace_buffer_void_t *) &(buf_), \
                                (chunk_size_))

#define LAPLACE_BUF_INDEX_UNSAFE_(buf_, id_) \
  (buf_).blocks.values[id_].index

#define LAPLACE_BUF_ZERO_(buf_, handle_, offset_, size_)      \
  do {                                                        \
    if ((handle_).id != LAPLACE_ID_UNDEFINED) {               \
      ptrdiff_t const begin_ = LAPLACE_BUF_INDEX_UNSAFE_(     \
                                   (buf_), (handle_).id) +    \
                               (offset_);                     \
      ptrdiff_t const end_ = begin_ + (size_);                \
      for (ptrdiff_t i_ = begin_; i_ < end_; i_++) {          \
        assert(i_ >= 0 && i_ < LAPLACE_BUF_DATA_(buf_).size); \
        atomic_store_explicit(                                \
            &LAPLACE_BUF_DATA_(buf_).values[i_].value, 0,     \
            memory_order_relaxed);                            \
        atomic_store_explicit(                                \
            &LAPLACE_BUF_DATA_(buf_).values[i_].delta, 0,     \
            memory_order_relaxed);                            \
      }                                                       \
    }                                                         \
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
    if (res_.status == LAPLACE_STATUS_OK) {                          \
      if (res_.offset != res_.previous_offset) {                     \
        for (ptrdiff_t i_ = 0;                                       \
             i_ < res_.previous_size && i_ < (size_); i_++) {        \
          assert(res_.offset + i_ >= 0 &&                            \
                 res_.offset + i_ < LAPLACE_BUF_DATA_(buf_).size);   \
          assert(res_.previous_offset + i_ >= 0 &&                   \
                 res_.previous_offset + i_ <                         \
                     LAPLACE_BUF_DATA_(buf_).size);                  \
          atomic_store_explicit(                                     \
              &LAPLACE_BUF_DATA_(buf_)                               \
                   .values[res_.offset + i_]                         \
                   .value,                                           \
              atomic_load_explicit(                                  \
                  &LAPLACE_BUF_DATA_(buf_)                           \
                       .values[res_.previous_offset + i_]            \
                       .value,                                       \
                  memory_order_relaxed),                             \
              memory_order_relaxed);                                 \
          atomic_store_explicit(                                     \
              &LAPLACE_BUF_DATA_(buf_)                               \
                   .values[res_.offset + i_]                         \
                   .delta,                                           \
              atomic_load_explicit(                                  \
                  &LAPLACE_BUF_DATA_(buf_)                           \
                       .values[res_.previous_offset + i_]            \
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

/*  Get buffer data block size by handle.
 *  May occur concurrently with buffer data reallocation.
 */
#define LAPLACE_BUFFER_SIZE(buf_, handle_) \
  laplace_buffer_size((laplace_buffer_void_t *) &(buf_), (handle_))

/*  Read buffer data block.
 *  May occur concurrently with buffer data reallocation.
 */
#define LAPLACE_BUFFER_READ(status_, buf_, handle_, index_, size_, \
                            dst_)                                  \
  do {                                                             \
    if (mtx_lock(&(buf_).read_lock) != thrd_success) {             \
      (status_) = LAPLACE_ERROR_BAD_MUTEX_LOCK;                    \
    } else {                                                       \
      (buf_).read_count++;                                         \
      (void) mtx_unlock(&(buf_).read_lock);                        \
      (status_) = laplace_buffer_check(                            \
          (laplace_buffer_void_t *) &(buf_), (handle_), (index_),  \
          (size_));                                                \
      if ((status_) == LAPLACE_STATUS_OK) {                        \
        for (ptrdiff_t i_ = 0; i_ < (size_); i_++) {               \
          (dst_)[i_] = atomic_load_explicit(                       \
              &(buf_)                                              \
                   .data                                           \
                   .values                                         \
                       [(buf_).blocks.values[(handle_).id].index + \
                        (index_) + i_]                             \
                   .value,                                         \
              memory_order_relaxed);                               \
        }                                                          \
      }                                                            \
      if (mtx_lock(&(buf_).read_lock) != thrd_success) {           \
        (status_) = LAPLACE_ERROR_BAD_MUTEX_LOCK;                  \
      } else {                                                     \
        (buf_).read_count--;                                       \
        (void) mtx_unlock(&(buf_).read_lock);                      \
      }                                                            \
    }                                                              \
  } while (0)

#define LAPLACE_BUFFER_SET_UNSAFE(buf_, handle_, index_, value_)    \
  do {                                                              \
    ptrdiff_t const offset_ = LAPLACE_BUF_INDEX_UNSAFE_(            \
                                  (buf_), (handle_).id) +           \
                              (index_);                             \
    assert(offset_ >= 0 && offset_ < LAPLACE_BUF_DATA_(buf_).size); \
    atomic_fetch_add_explicit(                                      \
        &LAPLACE_BUF_DATA_(buf_).values[offset_].delta,             \
        (value_) -atomic_load_explicit(                             \
            &LAPLACE_BUF_DATA_(buf_).values[offset_].value,         \
            memory_order_relaxed),                                  \
        memory_order_relaxed);                                      \
  } while (0)

#define LAPLACE_BUFFER_ADD_UNSAFE(buf_, handle_, index_, delta_)    \
  do {                                                              \
    ptrdiff_t const offset_ = LAPLACE_BUF_INDEX_UNSAFE_(            \
                                  (buf_), (handle_).id) +           \
                              (index_);                             \
    assert(offset_ >= 0 && offset_ < LAPLACE_BUF_DATA_(buf_).size); \
    atomic_fetch_add_explicit(                                      \
        &LAPLACE_BUF_DATA_(buf_).values[offset_].delta, (delta_),   \
        memory_order_relaxed);                                      \
  } while (0)

#define LAPLACE_BUFFER_SET(status_, buf_, handle_, index_, value_)  \
  do {                                                              \
    (status_) = laplace_buffer_check(                               \
        (laplace_buffer_void_t *) &(buf_), (handle_), (index_), 1); \
    if ((status_) == LAPLACE_STATUS_OK)                             \
      LAPLACE_BUFFER_SET_UNSAFE(buf_, handle_, index_, value_);     \
  } while (0)

#define LAPLACE_BUFFER_ADD(status_, buf_, handle_, index_, delta_)  \
  do {                                                              \
    (status_) = laplace_buffer_check(                               \
        (laplace_buffer_void_t *) &(buf_), (handle_), (index_), 1); \
    if ((status_) == LAPLACE_STATUS_OK)                             \
      LAPLACE_BUFFER_ADD_UNSAFE(buf_, handle_, index_, delta_);     \
  } while (0)

#define LAPLACE_BUFFER_SET_N(status_, buf_, handle_, index_, size_, \
                             values_)                               \
  do {                                                              \
    (status_) = laplace_buffer_check(                               \
        (laplace_buffer_void_t *) &(buf_), (handle_), (index_),     \
        (size_));                                                   \
    if ((status_) == LAPLACE_STATUS_OK)                             \
      for (ptrdiff_t i_ = 0; i_ < (size_); i_++)                    \
        LAPLACE_BUFFER_SET_UNSAFE(buf_, handle_, (index_) + i_,     \
                                  (values_)[i_]);                   \
  } while (0)

#define LAPLACE_BUFFER_ADD_N(status_, buf_, handle_, index_, size_, \
                             deltas_)                               \
  do {                                                              \
    (status_) = laplace_buffer_check(                               \
        (laplace_buffer_void_t *) &(buf_), (handle_), (index_),     \
        (size_));                                                   \
    if ((status_) == LAPLACE_STATUS_OK)                             \
      for (ptrdiff_t i_ = 0; i_ < (size_); i_++)                    \
        LAPLACE_BUFFER_ADD_UNSAFE(buf_, handle_, (index_) + i_,     \
                                  (deltas_)[i_]);                   \
  } while (0)

#define LAPLACE_BUFFER_ADJUST(return_, buffer_)                \
  do {                                                         \
    ptrdiff_t const begin_ = atomic_fetch_add_explicit(        \
        &(buffer_).next_chunk, (buffer_).chunk_size,           \
        memory_order_relaxed);                                 \
    ptrdiff_t end_ = begin_ + (buffer_).chunk_size;            \
    if (end_ > LAPLACE_BUF_DATA_(buffer_).size)                \
      end_ = LAPLACE_BUF_DATA_(buffer_).size;                  \
    for (ptrdiff_t i_ = begin_; i_ < end_; i_++) {             \
      assert(i_ >= 0 && i_ < LAPLACE_BUF_DATA_(buffer_).size); \
      atomic_fetch_add_explicit(                               \
          &LAPLACE_BUF_DATA_(buffer_).values[i_].value,        \
          atomic_exchange_explicit(                            \
              &LAPLACE_BUF_DATA_(buffer_).values[i_].delta, 0, \
              memory_order_relaxed),                           \
          memory_order_relaxed);                               \
    }                                                          \
    (return_) = (end_ != LAPLACE_BUF_DATA_(buffer_).size);     \
  } while (0)

#define LAPLACE_BUFFER_ADJUST_LOOP(buffer_)       \
  do {                                            \
    for (int running_ = 1; running_;)             \
      LAPLACE_BUFFER_ADJUST(running_, (buffer_)); \
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
    (dst).cell_size  = (src).cell_size;                           \
    (dst).chunk_size = (src).chunk_size;                          \
    (dst).reserved   = (src).reserved;                            \
    (dst).next_block = (src).next_block;                          \
    atomic_store_explicit(&(dst).next_chunk, 0,                   \
                          memory_order_relaxed);                  \
    KIT_DA_INIT((dst).info, (src).info.size, (src).info.alloc);   \
    if ((dst).info.size != (src).info.size)                       \
      (s) = LAPLACE_ERROR_BAD_ALLOC;                              \
    else                                                          \
      memcpy((dst).info.values, (src).info.values,                \
             sizeof((src).info.values[0]) * (src).info.size);     \
    KIT_DA_INIT((dst).blocks, (src).blocks.size,                  \
                (src).blocks.alloc);                              \
    if ((dst).blocks.size != (src).blocks.size)                   \
      (s) = LAPLACE_ERROR_BAD_ALLOC;                              \
    else                                                          \
      memcpy((dst).blocks.values, (src).blocks.values,            \
             sizeof((src).blocks.values[0]) * (src).blocks.size); \
    KIT_DA_INIT((dst).data, (src).data.size, (src).data.alloc);   \
    if ((dst).data.size != (src).data.size)                       \
      (s) = LAPLACE_ERROR_BAD_ALLOC;                              \
    else {                                                        \
      for (ptrdiff_t i_ = 0; i_ < (src).data.size; i_++) {        \
        atomic_store_explicit(                                    \
            &(dst).data.values[i_].value,                         \
            atomic_load_explicit(&(src).data.values[i_].value,    \
                                 memory_order_relaxed),           \
            memory_order_relaxed);                                \
        atomic_store_explicit(&(dst).data.values[i_].delta, 0,    \
                              memory_order_relaxed);              \
      }                                                           \
    }                                                             \
    (s) = LAPLACE_STATUS_OK;                                      \
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
#  define BUFFER_SIZE LAPLACE_BUFFER_SIZE
#  define BUFFER_READ LAPLACE_BUFFER_READ
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
