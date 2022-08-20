#ifndef LAPLACE_ACCESS_H
#define LAPLACE_ACCESS_H

#include "handle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct laplace_impact     laplace_impact_t;
typedef struct laplace_read_write laplace_read_write_t;

typedef void (*laplace_acquire_fn)(void *state);
typedef void (*laplace_release_fn)(void *state);

typedef laplace_status_t (*laplace_clone_fn)(
    void *state, laplace_read_write_t *cloned);

typedef ptrdiff_t (*laplace_integers_size_fn)(
    void *state, laplace_handle_t handle);

typedef ptrdiff_t (*laplace_bytes_size_fn)(void            *state,
                                           laplace_handle_t handle);

typedef laplace_status_t (*laplace_read_integers_fn)(
    void *state, laplace_handle_t handle, ptrdiff_t index,
    ptrdiff_t size, laplace_integer_t *destination);

typedef laplace_status_t (*laplace_read_bytes_fn)(
    void *state, laplace_handle_t handle, ptrdiff_t index,
    ptrdiff_t size, laplace_byte_t *destination);

typedef laplace_integer_t (*laplace_get_integer_fn)(
    void *state, laplace_handle_t handle, ptrdiff_t index,
    laplace_integer_t invalid);

typedef laplace_byte_t (*laplace_get_byte_fn)(void            *state,
                                              laplace_handle_t handle,
                                              ptrdiff_t        index,
                                              laplace_byte_t invalid);

typedef laplace_status_t (*laplace_apply_fn)(
    void *state, laplace_impact_t const *impact);

typedef void (*laplace_adjust_loop_fn)(void *state);

typedef void (*laplace_adjust_done_fn)(void *state);

struct laplace_read_write {
  void              *state;
  laplace_acquire_fn acquire;
  laplace_release_fn release;
  laplace_clone_fn   clone;

  laplace_integers_size_fn integers_size;
  laplace_bytes_size_fn    bytes_size;
  laplace_read_integers_fn read_integers;
  laplace_read_bytes_fn    read_bytes;
  laplace_get_integer_fn   get_integer;
  laplace_get_byte_fn      get_byte;

  laplace_apply_fn       apply;
  laplace_adjust_loop_fn adjust_loop;
  laplace_adjust_done_fn adjust_done;
};

typedef struct {
  void              *state;
  laplace_acquire_fn acquire;
  laplace_release_fn release;

  laplace_integers_size_fn integers_size;
  laplace_integers_size_fn bytes_size;
  laplace_read_integers_fn read_integers;
  laplace_read_bytes_fn    read_bytes;
  laplace_get_integer_fn   get_integer;
  laplace_get_byte_fn      get_byte;
} laplace_read_only_t;

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define impact_t laplace_impact_t
#  define read_write_t laplace_read_write_t
#  define read_only_t laplace_read_only_t
#endif

#ifdef __cplusplus
}
#endif

#endif
