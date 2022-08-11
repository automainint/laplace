#include "buffer.h"

void laplace_buffer_set_chunk_size(
    laplace_buffer_void_t *const buffer, ptrdiff_t const chunk_size) {
  if (buffer == NULL)
    return;

  if (chunk_size <= 0)
    buffer->error = LAPLACE_BUFFER_ERROR_INVALID_CHUNK_SIZE;
  else
    buffer->chunk_size = chunk_size;
}

laplace_buffer_handle_t laplace_buffer_allocate(
    laplace_buffer_void_t *const buffer, ptrdiff_t const element_size,
    ptrdiff_t const size) {
  laplace_buffer_handle_t h = { .id = 0, .generation = 0 };

  if (buffer->error) {
    h.id    = ID_UNDEFINED;
    h.error = LAPLACE_BUFFER_ERROR_INVALID_BUFFER;
  } else if (size <= 0 || size >= PTRDIFF_MAX) {
    h.id    = ID_UNDEFINED;
    h.error = LAPLACE_BUFFER_ERROR_INVALID_SIZE;
  } else {
    buffer->generation++;
  }
  return h;
}

laplace_buffer_handle_t laplace_buffer_allocate_into(
    laplace_buffer_void_t *const buffer, ptrdiff_t const element_size,
    ptrdiff_t const size, laplace_buffer_handle_t handle) {
  if (buffer->error) {
    handle.id    = ID_UNDEFINED;
    handle.error = LAPLACE_BUFFER_ERROR_INVALID_BUFFER;
  } else if (handle.id == ID_UNDEFINED) {
    handle.error = LAPLACE_BUFFER_ERROR_INVALID_HANDLE_ID;
  } else if (handle.generation != buffer->generation) {
    handle.id    = ID_UNDEFINED;
    handle.error = LAPLACE_BUFFER_ERROR_INVALID_HANDLE_GENERATION;
  } else if (size <= 0) {
    handle.id    = ID_UNDEFINED;
    handle.error = LAPLACE_BUFFER_ERROR_INVALID_SIZE;
  } else {
    buffer->generation++;
    handle.generation++;
  }
  return handle;
}
