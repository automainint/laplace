#ifndef LAPLACE_BUFFER_H
#define LAPLACE_BUFFER_H

#include "options.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  LAPLACE_BUFFER_ERROR_INVALID_CHUNK_SIZE        = 1,
  LAPLACE_BUFFER_ERROR_INVALID_HANDLE_ID         = 2,
  LAPLACE_BUFFER_ERROR_INVALID_HANDLE_GENERATION = 3,
  LAPLACE_BUFFER_ERROR_INVALID_SIZE              = 4,
  LAPLACE_BUFFER_ERROR_INVALID_BUFFER            = 5,
  LAPLACE_BUFFER_DEFAULT_CHUNK_SIZE              = 8000
};

typedef struct {
  ptrdiff_t id;
  union {
    int       error;
    ptrdiff_t generation;
  };
} laplace_buffer_handle_t;

typedef struct {
  int       error;
  ptrdiff_t chunk_size;
  ptrdiff_t generation;
  void     *values;
} laplace_buffer_void_t;

#define LAPLACE_BUFFER_CREATE(name_, element_type_)        \
  struct {                                                 \
    int       error;                                       \
    ptrdiff_t chunk_size;                                  \
    ptrdiff_t generation;                                  \
    struct {                                               \
      element_type_ value;                                 \
    } * values;                                            \
  } name_;                                                 \
  do {                                                     \
    memset(&name_, 0, sizeof name_);                       \
    name_.chunk_size = LAPLACE_BUFFER_DEFAULT_CHUNK_SIZE / \
                       sizeof(element_type_);              \
    name_.generation = -1;                                 \
  } while (0)

void laplace_buffer_set_chunk_size(laplace_buffer_void_t *buffer,
                                   ptrdiff_t              chunk_size);

laplace_buffer_handle_t laplace_buffer_allocate(
    laplace_buffer_void_t *buffer, ptrdiff_t element_size,
    ptrdiff_t size);

laplace_buffer_handle_t laplace_buffer_allocate_into(
    laplace_buffer_void_t *buffer, ptrdiff_t element_size,
    ptrdiff_t size, laplace_buffer_handle_t handle);

#define LAPLACE_BUFFER_ERROR(buf_) (buf_).error

#define LAPLACE_BUFFER_CHUNK_SIZE(buf_) (buf_).chunk_size

#define LAPLACE_BUFFER_SET_CHUNK_SIZE(buf_, chunk_size_)           \
  laplace_buffer_set_chunk_size((laplace_buffer_void_t *) &(buf_), \
                                (chunk_size_))

#define LAPLACE_BUFFER_ALLOCATE(buf_, size_)                 \
  laplace_buffer_allocate((laplace_buffer_void_t *) &(buf_), \
                          sizeof((buf_).values[0].value), (size_))

#define LAPLACE_BUFFER_ALLOCATE_INTO(buf_, size_, handle_)        \
  laplace_buffer_allocate_into((laplace_buffer_void_t *) &(buf_), \
                               sizeof((buf_).values[0].value),    \
                               (size_), (handle_))

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define BUFFER_CREATE LAPLACE_BUFFER_CREATE
#  define BUFFER_ERROR LAPLACE_BUFFER_ERROR
#  define BUFFER_CHUNK_SIZE LAPLACE_BUFFER_CHUNK_SIZE
#  define BUFFER_SET_CHUNK_SIZE LAPLACE_BUFFER_SET_CHUNK_SIZE
#  define BUFFER_ALLOCATE LAPLACE_BUFFER_ALLOCATE
#  define BUFFER_ALLOCATE_INTO LAPLACE_BUFFER_ALLOCATE_INTO

#  define BUFFER_DEFAULT_CHUNK_SIZE LAPLACE_BUFFER_DEFAULT_CHUNK_SIZE
#  define BUFFER_ERROR_INVALID_CHUNK_SIZE \
    LAPLACE_BUFFER_ERROR_INVALID_CHUNK_SIZE

#  define buffer_handle_t laplace_buffer_handle_t
#  define buffer_chunk_size laplace_buffer_chunk_size
#endif

#ifdef __cplusplus
}
#endif

#endif
