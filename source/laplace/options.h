#ifndef LAPLACE_OPTIONS_H
#define LAPLACE_OPTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef int64_t laplace_time_t;
typedef int64_t laplace_integer_t;
typedef int8_t  laplace_byte_t;

enum { LAPLACE_ID_UNDEFINED = -1 };

typedef enum {
  LAPLACE_STATUS_OK = 0,
  LAPLACE_ERROR_BAD_ALLOC,
  LAPLACE_ERROR_BAD_MUTEX_INIT,
  LAPLACE_ERROR_BAD_MUTEX_LOCK,
  LAPLACE_ERROR_BAD_MUTEX_UNLOCK,
  LAPLACE_ERROR_BAD_CNDVAR_INIT,
  LAPLACE_ERROR_BAD_CNDVAR_BROADCAST,
  LAPLACE_ERROR_BAD_CNDVAR_WAIT,
  LAPLACE_ERROR_BAD_THREAD_CREATE,
  LAPLACE_ERROR_INVALID_CHUNK_SIZE,
  LAPLACE_ERROR_INVALID_HANDLE_ID,
  LAPLACE_ERROR_INVALID_HANDLE_GENERATION,
  LAPLACE_ERROR_INVALID_INDEX,
  LAPLACE_ERROR_INVALID_SIZE,
  LAPLACE_ERROR_INVALID_BUFFER,
  LAPLACE_ERROR_WRONG_IMPACT,
  LAPLACE_ERROR_NO_THREAD_POOL
} laplace_status_t;

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define ID_UNDEFINED LAPLACE_ID_UNDEFINED
#  define STATUS_OK LAPLACE_STATUS_OK
#  define ERROR_BAD_ALLOC LAPLACE_ERROR_BAD_ALLOC
#  define ERROR_BAD_MUTEX_INIT LAPLACE_ERROR_BAD_MUTEX_INIT
#  define ERROR_BAD_MUTEX_LOCK LAPLACE_ERROR_BAD_MUTEX_LOCK
#  define ERROR_BAD_MUTEX_UNLOCK LAPLACE_ERROR_BAD_MUTEX_UNLOCK
#  define ERROR_BAD_CNDVAR_INIT LAPLACE_ERROR_BAD_CNDVAR_INIT
#  define ERROR_BAD_CNDVAR_BROADCAST \
    LAPLACE_ERROR_BAD_CNDVAR_BROADCAST
#  define ERROR_BAD_CNDVAR_WAIT LAPLACE_ERROR_BAD_CNDVAR_WAIT
#  define ERROR_BAD_THREAD_CREATE LAPLACE_ERROR_BAD_THREAD_CREATE
#  define ERROR_INVALID_CHUNK_SIZE LAPLACE_ERROR_INVALID_CHUNK_SIZE
#  define ERROR_INVALID_HANDLE_ID LAPLACE_ERROR_INVALID_HANDLE_ID
#  define ERROR_INVALID_HANDLE_GENERATION \
    LAPLACE_ERROR_INVALID_HANDLE_GENERATION
#  define ERROR_INVALID_INDEX LAPLACE_ERROR_INVALID_INDEX
#  define ERROR_INVALID_SIZE LAPLACE_ERROR_INVALID_SIZE
#  define ERROR_INVALID_BUFFER LAPLACE_ERROR_INVALID_BUFFER
#  define ERROR_WRONG_IMPACT LAPLACE_ERROR_WRONG_IMPACT
#  define ERROR_NO_THREAD_POOL LAPLACE_ERROR_NO_THREAD_POOL
#endif

#ifdef __cplusplus
}

namespace laplace {
  using time_type = int64_t;
  using int_type  = int64_t;
  using byte_type = int8_t;

  static constexpr ptrdiff_t id_undefined    = -1;
  static constexpr ptrdiff_t index_undefined = -1;

  [[nodiscard]] auto default_thread_count() noexcept -> ptrdiff_t;

  [[nodiscard]] auto overthreading_limit() noexcept -> ptrdiff_t;

  [[nodiscard]] auto average_impact_list_size() noexcept -> ptrdiff_t;

  [[nodiscard]] auto average_async_impact_count() noexcept
      -> ptrdiff_t;

  [[nodiscard]] auto average_sync_impact_count() noexcept
      -> ptrdiff_t;

  [[nodiscard]] auto average_fork_count() noexcept -> ptrdiff_t;
}
#endif

#endif
