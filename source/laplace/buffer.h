#ifndef LAPLACE_BUFFER_H
#define LAPLACE_BUFFER_H

#include "handle.h"

#include <kit/atomic.h>
#include <kit/dynamic_array.h>

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  LAPLACE_BUFFER_ERROR_INVALID_CHUNK_SIZE = 1,
  LAPLACE_BUFFER_ERROR_INVALID_HANDLE_ID,
  LAPLACE_BUFFER_ERROR_INVALID_HANDLE_GENERATION,
  LAPLACE_BUFFER_ERROR_INVALID_INDEX,
  LAPLACE_BUFFER_ERROR_INVALID_SIZE,
  LAPLACE_BUFFER_ERROR_INVALID_BUFFER,
  LAPLACE_BUFFER_ERROR_BAD_ALLOC,
  LAPLACE_BUFFER_DEFAULT_CHUNK_SIZE = 8000
};

#define LAPLACE_BUFFER_BLOCK \
  struct {                   \
    ptrdiff_t index;         \
    ptrdiff_t generation;    \
    ptrdiff_t size;          \
  }

#define LAPLACE_BUFFER_DATA               \
  struct {                                \
    ptrdiff_t chunk_size;                 \
    ptrdiff_t reserved;                   \
    ptrdiff_t next_block;                 \
    ATOMIC(ptrdiff_t) next_chunk;         \
    KIT_DA(blocks, LAPLACE_BUFFER_BLOCK); \
  }

#define LAPLACE_BUFFER_CELL_DATA \
  struct {                       \
    int       empty;             \
    ptrdiff_t offset;            \
  }

#define LAPLACE_BUFFER_CELL(element_type_) \
  struct {                                 \
    LAPLACE_BUFFER_CELL_DATA;              \
    ATOMIC(element_type_) delta;           \
    element_type_ value;                   \
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

#define LAPLACE_BUFFER_INIT(buf_, alloc_)                    \
  do {                                                       \
    memset(&(buf_), 0, sizeof(buf_));                        \
    (buf_).chunk_size = LAPLACE_BUFFER_DEFAULT_CHUNK_SIZE /  \
                        sizeof((buf_).data.values[0].value); \
    KIT_DA_INIT((buf_).blocks, 0, (alloc_));                 \
    KIT_DA_INIT((buf_).data, 0, (alloc_));                   \
  } while (0)

#define LAPLACE_BUFFER_CREATE(name_, element_type_) \
  LAPLACE_BUFFER_TYPE(element_type_) name_;         \
  LAPLACE_BUFFER_INIT(name_, kit_alloc_default())

#define LAPLACE_BUFFER_DESTROY(buffer_) \
  do {                                  \
    KIT_DA_DESTROY((buffer_).blocks);   \
    KIT_DA_DESTROY((buffer_).data);     \
  } while (0)

laplace_status_t laplace_buffer_set_chunk_size(
    laplace_buffer_void_t *buffer, ptrdiff_t chunk_size);

laplace_handle_t laplace_buffer_allocate(
    laplace_buffer_void_t *buffer, ptrdiff_t cell_size,
    ptrdiff_t size);

laplace_handle_t laplace_buffer_allocate_into(
    laplace_buffer_void_t *buffer, ptrdiff_t cell_size,
    ptrdiff_t size, laplace_handle_t handle);

laplace_status_t laplace_buffer_reserve(laplace_buffer_void_t *buffer,
                                        ptrdiff_t              size);

laplace_status_t laplace_buffer_deallocate(
    laplace_buffer_void_t *buffer, ptrdiff_t cell_size,
    laplace_handle_t handle);

laplace_status_t laplace_buffer_check(laplace_buffer_void_t *buffer,
                                      ptrdiff_t        cell_size,
                                      laplace_handle_t handle,
                                      ptrdiff_t        index,
                                      ptrdiff_t        size);

#define LAPLACE_BUFFER_SET_CHUNK_SIZE(buf_, chunk_size_)           \
  laplace_buffer_set_chunk_size((laplace_buffer_void_t *) &(buf_), \
                                (chunk_size_))

#define LAPLACE_BUFFER_ZERO(buf_, handle_)                      \
  do {                                                          \
    if ((handle_).id != LAPLACE_ID_UNDEFINED) {                 \
      ptrdiff_t const begin_ =                                  \
          (buf_).blocks.values[(handle_).id].index;             \
      ptrdiff_t const end_ =                                    \
          begin_ + (buf_).blocks.values[(handle_).id].size;     \
      for (ptrdiff_t i_ = begin_; i_ < end_; i_++) {            \
        (buf_).data.values[i_].value = 0;                       \
        atomic_store_explicit(&(buf_).data.values[i_].delta, 0, \
                              memory_order_relaxed);            \
      }                                                         \
    }                                                           \
  } while (0)

#define LAPLACE_BUFFER_ALLOCATE(return_, buf_, size_) \
  do {                                                \
    (return_) = laplace_buffer_allocate(              \
        (laplace_buffer_void_t *) &(buf_),            \
        sizeof((buf_).data.values[0]), (size_));      \
    LAPLACE_BUFFER_ZERO((buf_), (return_));           \
  } while (0)

#define LAPLACE_BUFFER_ALLOCATE_INTO(return_, buf_, size_, handle_) \
  do {                                                              \
    (return_) = laplace_buffer_allocate_into(                       \
        (laplace_buffer_void_t *) &(buf_),                          \
        sizeof((buf_).data.values[0]), (size_), (handle_));         \
    LAPLACE_BUFFER_ZERO((buf_), (return_));                         \
  } while (0)

#define LAPLACE_BUFFER_RESERVE(buf_, size_) \
  laplace_buffer_reserve((laplace_buffer_void_t *) &(buf_), (size_))

#define LAPLACE_BUFFER_DEALLOCATE(buf_, handle_)               \
  laplace_buffer_deallocate((laplace_buffer_void_t *) &(buf_), \
                            sizeof((buf_).data.values[0]),     \
                            (handle_))

#define LAPLACE_BUFFER_GET(buf_, handle_, index_, invalid_)       \
  (laplace_buffer_check((laplace_buffer_void_t *) &(buf_),        \
                        sizeof((buf_).data.values[0]), (handle_), \
                        (index_), 1) == STATUS_OK                 \
       ? (buf_)                                                   \
             .data                                                \
             .values[(buf_).blocks.values[(handle_).id].index +   \
                     (index_)]                                    \
             .value                                               \
       : (invalid_))

#define LAPLACE_BUFFER_SET(status_, buf_, handle_, index_, value_) \
  do {                                                             \
    (status_) = laplace_buffer_check(                              \
        (laplace_buffer_void_t *) &(buf_),                         \
        sizeof((buf_).data.values[0]), (handle_), (index_), 1);    \
    if ((status_) == LAPLACE_STATUS_OK) {                          \
      ptrdiff_t const offset_ =                                    \
          (buf_).blocks.values[(handle_).id].index + (index_);     \
      atomic_fetch_add_explicit(                                   \
          &(buf_).data.values[offset_].delta,                      \
          (value_) - (buf_).data.values[offset_].value,            \
          memory_order_relaxed);                                   \
    }                                                              \
  } while (0)

#define LAPLACE_BUFFER_ADD(status_, buf_, handle_, index_, delta_)  \
  do {                                                              \
    (status_) = laplace_buffer_check(                               \
        (laplace_buffer_void_t *) &(buf_),                          \
        sizeof((buf_).data.values[0]), (handle_), (index_), 1);     \
    if ((status_) == LAPLACE_STATUS_OK) {                           \
      ptrdiff_t const offset_ =                                     \
          (buf_).blocks.values[(handle_).id].index + (index_);      \
      atomic_fetch_add_explicit(&(buf_).data.values[offset_].delta, \
                                (delta_), memory_order_relaxed);    \
    }                                                               \
  } while (0)

#define LAPLACE_BUFFER_ADJUST(return_, buffer_)                    \
  do {                                                             \
    ptrdiff_t const begin_ = atomic_fetch_add_explicit(            \
        &(buffer_).next_chunk, (buffer_).chunk_size,               \
        memory_order_relaxed);                                     \
    ptrdiff_t end_ = begin_ + (buffer_).chunk_size;                \
    if (end_ > (buffer_).data.size)                                \
      end_ = (buffer_).data.size;                                  \
    for (ptrdiff_t i_ = begin_; i_ < end_; i_++) {                 \
      (buffer_).data.values[i_].value += atomic_exchange_explicit( \
          &(buffer_).data.values[i_].delta, 0,                     \
          memory_order_relaxed);                                   \
    }                                                              \
    (return_) = (end_ != (buffer_).data.size);                     \
  } while (0)

#define LAPLACE_BUFFER_ADJUST_LOOP(buffer_)       \
  do {                                            \
    for (int running_ = 1; running_;)             \
      LAPLACE_BUFFER_ADJUST(running_, (buffer_)); \
  } while (0)

#define LAPLACE_BUFFER_ADJUST_DONE(buffer_)       \
  atomic_store_explicit(&(buffer_).next_chunk, 0, \
                        memory_order_relaxed)

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define BUFFER_TYPE LAPLACE_BUFFER_TYPE
#  define BUFFER_INIT LAPLACE_BUFFER_INIT
#  define BUFFER_CREATE LAPLACE_BUFFER_CREATE
#  define BUFFER_DESTROY LAPLACE_BUFFER_DESTROY
#  define BUFFER_SET_CHUNK_SIZE LAPLACE_BUFFER_SET_CHUNK_SIZE
#  define BUFFER_ALLOCATE LAPLACE_BUFFER_ALLOCATE
#  define BUFFER_ALLOCATE_INTO LAPLACE_BUFFER_ALLOCATE_INTO
#  define BUFFER_RESERVE LAPLACE_BUFFER_RESERVE
#  define BUFFER_DEALLOCATE LAPLACE_BUFFER_DEALLOCATE
#  define BUFFER_GET LAPLACE_BUFFER_GET
#  define BUFFER_SET LAPLACE_BUFFER_SET
#  define BUFFER_ADD LAPLACE_BUFFER_ADD
#  define BUFFER_ADJUST LAPLACE_BUFFER_ADJUST
#  define BUFFER_ADJUST_LOOP LAPLACE_BUFFER_ADJUST_LOOP
#  define BUFFER_ADJUST_DONE LAPLACE_BUFFER_ADJUST_DONE

#  define BUFFER_ERROR_INVALID_CHUNK_SIZE \
    LAPLACE_BUFFER_ERROR_INVALID_CHUNK_SIZE
#  define BUFFER_ERROR_INVALID_HANDLE_ID \
    LAPLACE_BUFFER_ERROR_INVALID_HANDLE_ID
#  define BUFFER_ERROR_INVALID_HANDLE_GENERATION \
    LAPLACE_BUFFER_ERROR_INVALID_HANDLE_GENERATION
#  define BUFFER_ERROR_INVALID_INDEX \
    LAPLACE_BUFFER_ERROR_INVALID_INDEX
#  define BUFFER_ERROR_INVALID_SIZE LAPLACE_BUFFER_ERROR_INVALID_SIZE
#  define BUFFER_ERROR_INVALID_BUFFER \
    LAPLACE_BUFFER_ERROR_INVALID_BUFFER
#  define BUFFER_ERROR_BAD_ALLOC LAPLACE_BUFFER_ERROR_BAD_ALLOC
#  define BUFFER_DEFAULT_CHUNK_SIZE LAPLACE_BUFFER_DEFAULT_CHUNK_SIZE
#endif

#ifdef __cplusplus
}
#endif

#endif
