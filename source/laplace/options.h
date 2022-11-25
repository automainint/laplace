#ifndef LAPLACE_OPTIONS_H
#define LAPLACE_OPTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <kit/options.h>

#include <stddef.h>
#include <stdint.h>

typedef int64_t laplace_time_t;
typedef int64_t laplace_integer_t;
typedef int8_t  laplace_byte_t;

enum { LAPLACE_ID_UNDEFINED = -1, LAPLACE_COROUTINE_SIZE = 400 };

enum {
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
  LAPLACE_ERROR_INVALID_EVENT_TIME,
  LAPLACE_ERROR_WRONG_IMPACT,
  LAPLACE_ERROR_NO_THREAD_POOL,
  LAPLACE_ERROR_NOT_IMPLEMENTED = -1
};

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define ID_UNDEFINED LAPLACE_ID_UNDEFINED
#  define COROUTINE_SIZE LAPLACE_COROUTINE_SIZE
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
#  define ERROR_INVALID_EVENT_TIME LAPLACE_ERROR_INVALID_EVENT_TIME
#  define ERROR_WRONG_IMPACT LAPLACE_ERROR_WRONG_IMPACT
#  define ERROR_NO_THREAD_POOL LAPLACE_ERROR_NO_THREAD_POOL
#  define ERROR_NOT_IMPLEMENTED LAPLACE_ERROR_NOT_IMPLEMENTED
#endif

#ifdef __cplusplus
}
#endif

#endif
