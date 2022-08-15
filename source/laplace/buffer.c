#include "buffer.h"

/*  TODO
 *  buffer_realloc
 */

static ptrdiff_t buffer_alloc(laplace_buffer_void_t *const buffer,
                              ptrdiff_t const              cell_size,
                              ptrdiff_t const              size) {
  ptrdiff_t offset = 0;

  while (offset < buffer->data.size) {
    laplace_buf_info_t_ *info = LAPLACE_BUF_INFO_(*buffer).values +
                                offset;
    if (info->empty && info->offset >= size)
      break;
    offset += info->offset;
  }

  if (size >= PTRDIFF_MAX - offset)
    return LAPLACE_ID_UNDEFINED;

  if (offset >= buffer->data.size) {
    ptrdiff_t i = buffer->data.size;

    DA_RESIZE(buffer->info, offset + size);
    if (buffer->info.size != offset + size)
      return LAPLACE_ID_UNDEFINED;

    da_resize((da_void_t *) &buffer->data, cell_size, offset + size);
    if (buffer->data.size != offset + size)
      return LAPLACE_ID_UNDEFINED;
  }

  for (ptrdiff_t i = 0; i < size; i++) {
    laplace_buf_info_t_ *info = LAPLACE_BUF_INFO_(*buffer).values +
                                offset + i;
    info->empty  = 0;
    info->offset = size - i;
  }

  return offset;
}

static void buffer_free(laplace_buffer_void_t *const buffer,
                        ptrdiff_t const              offset) {
  ptrdiff_t begin = offset;
  ptrdiff_t end   = begin;

  while (end < buffer->data.size) {
    end += LAPLACE_BUF_INFO_(*buffer).values[end].offset;
    if (end < buffer->data.size &&
        !LAPLACE_BUF_INFO_(*buffer).values[end].empty)
      break;
  }

  for (ptrdiff_t i = begin; i < end; i++) {
    laplace_buf_info_t_ *info = LAPLACE_BUF_INFO_(*buffer).values + i;
    info->empty               = 1;
    info->offset              = end - i;
  }
}

laplace_status_t laplace_buffer_set_chunk_size(
    laplace_buffer_void_t *const buffer, ptrdiff_t const chunk_size) {
  if (buffer == NULL)
    return LAPLACE_BUFFER_ERROR_INVALID_BUFFER;
  if (chunk_size <= 0)
    return LAPLACE_BUFFER_ERROR_INVALID_CHUNK_SIZE;

  buffer->chunk_size = chunk_size;
  return STATUS_OK;
}

laplace_handle_t laplace_buffer_allocate(
    laplace_buffer_void_t *const buffer, ptrdiff_t const cell_size,
    ptrdiff_t const size) {
  laplace_handle_t h;

  if (size <= 0) {
    h.id    = LAPLACE_ID_UNDEFINED,
    h.error = LAPLACE_BUFFER_ERROR_INVALID_SIZE;
    return h;
  }

  ptrdiff_t const offset = buffer_alloc(buffer, cell_size, size);

  if (offset == LAPLACE_ID_UNDEFINED) {
    h.id    = LAPLACE_ID_UNDEFINED,
    h.error = LAPLACE_BUFFER_ERROR_BAD_ALLOC;
    return h;
  }

  ptrdiff_t const block = buffer->next_block;
  if (block >= buffer->blocks.size) {
    ptrdiff_t i = buffer->blocks.size;
    DA_RESIZE(buffer->blocks, block + 1);

    if (buffer->blocks.size != block + 1) {
      buffer_free(buffer, offset);
      h.id    = LAPLACE_ID_UNDEFINED,
      h.error = LAPLACE_BUFFER_ERROR_BAD_ALLOC;
      return h;
    }

    for (; i < buffer->blocks.size; i++) {
      buffer->blocks.values[i].index      = LAPLACE_ID_UNDEFINED;
      buffer->blocks.values[i].generation = -1;
    }
  }

  buffer->blocks.values[block].index = offset;
  buffer->blocks.values[block].generation++;
  buffer->blocks.values[block].size = size;

  while (buffer->next_block < buffer->blocks.size &&
         buffer->blocks.values[buffer->next_block].index !=
             LAPLACE_ID_UNDEFINED)
    buffer->next_block++;

  h.id         = block,
  h.generation = buffer->blocks.values[block].generation;
  return h;
}

laplace_handle_t laplace_buffer_allocate_into(
    laplace_buffer_void_t *const buffer, ptrdiff_t const cell_size,
    ptrdiff_t const size, laplace_handle_t handle) {
  laplace_handle_t h;

  if (handle.id < 0) {
    h.id    = LAPLACE_ID_UNDEFINED;
    h.error = LAPLACE_BUFFER_ERROR_INVALID_HANDLE_ID;
    return h;
  }

  if ((handle.id < buffer->blocks.size &&
       buffer->blocks.values[handle.id].generation !=
           handle.generation) ||
      (handle.id >= buffer->blocks.size && handle.generation != -1)) {
    h.id    = LAPLACE_ID_UNDEFINED;
    h.error = LAPLACE_BUFFER_ERROR_INVALID_HANDLE_GENERATION;
    return h;
  }

  if (size <= 0) {
    h.id    = LAPLACE_ID_UNDEFINED;
    h.error = LAPLACE_BUFFER_ERROR_INVALID_SIZE;
    return h;
  }

  if (handle.id < buffer->blocks.size &&
      buffer->blocks.values[handle.id].index != LAPLACE_ID_UNDEFINED)
    buffer_free(buffer, buffer->blocks.values[handle.id].index);

  ptrdiff_t const offset = buffer_alloc(buffer, cell_size, size);

  if (offset == LAPLACE_ID_UNDEFINED) {
    h.id    = LAPLACE_ID_UNDEFINED;
    h.error = LAPLACE_BUFFER_ERROR_BAD_ALLOC;
    return h;
  }

  if (handle.id >= buffer->blocks.size) {
    ptrdiff_t i = buffer->blocks.size;
    DA_RESIZE(buffer->blocks, handle.id + 1);

    if (buffer->blocks.size != handle.id + 1) {
      buffer_free(buffer, offset);
      h.id    = LAPLACE_ID_UNDEFINED;
      h.error = LAPLACE_BUFFER_ERROR_BAD_ALLOC;
      return h;
    }

    for (; i < buffer->blocks.size; i++) {
      buffer->blocks.values[i].index      = LAPLACE_ID_UNDEFINED;
      buffer->blocks.values[i].generation = -1;
    }
  }

  buffer->blocks.values[handle.id].index = offset;
  buffer->blocks.values[handle.id].generation++;
  buffer->blocks.values[handle.id].size = size;

  while (buffer->next_block < buffer->blocks.size &&
         buffer->blocks.values[buffer->next_block].index !=
             LAPLACE_ID_UNDEFINED)
    buffer->next_block++;

  h.id         = handle.id;
  h.generation = buffer->blocks.values[handle.id].generation;
  return h;
}

laplace_buffer_realloc_result_t laplace_buffer_reallocate(
    laplace_buffer_void_t *buffer, ptrdiff_t cell_size,
    ptrdiff_t size, laplace_handle_t handle) {
  laplace_buffer_realloc_result_t result;
  memset(&result, 0, sizeof result);

  if (size <= 0) {
    result.status = LAPLACE_BUFFER_ERROR_INVALID_SIZE;
    return result;
  }

  if (handle.id < 0 || handle.id >= buffer->blocks.size) {
    result.status = LAPLACE_BUFFER_ERROR_INVALID_HANDLE_ID;
    return result;
  }

  if (handle.generation !=
      buffer->blocks.values[handle.id].generation) {
    result.status = LAPLACE_BUFFER_ERROR_INVALID_HANDLE_GENERATION;
    return result;
  }

  ptrdiff_t const offset = buffer_alloc(buffer, cell_size, size);

  if (offset == LAPLACE_ID_UNDEFINED) {
    result.status = LAPLACE_BUFFER_ERROR_BAD_ALLOC;
    return result;
  }

  result.offset          = offset;
  result.previous_offset = buffer->blocks.values[handle.id].index;
  result.previous_size   = buffer->blocks.values[handle.id].size;

  buffer_free(buffer, buffer->blocks.values[handle.id].index);
  
  buffer->blocks.values[handle.id].index = offset;
  buffer->blocks.values[handle.id].size  = size;

  result.status = LAPLACE_STATUS_OK;
  return result;
}

laplace_status_t laplace_buffer_reserve(
    laplace_buffer_void_t *const buffer, ptrdiff_t const size) {
  if (size < 0)
    return LAPLACE_BUFFER_ERROR_INVALID_SIZE;

  if (buffer->blocks.size < size) {
    ptrdiff_t i = buffer->blocks.size;

    DA_RESIZE(buffer->blocks, size);
    if (buffer->blocks.size != size)
      return LAPLACE_BUFFER_ERROR_BAD_ALLOC;

    for (; i < buffer->blocks.size; i++) {
      buffer->blocks.values[i].index      = LAPLACE_ID_UNDEFINED;
      buffer->blocks.values[i].generation = -1;
    }
  }

  buffer->reserved = size;

  if (buffer->next_block < size)
    buffer->next_block = size;

  while (buffer->next_block < buffer->blocks.size &&
         buffer->blocks.values[buffer->next_block].index ==
             LAPLACE_ID_UNDEFINED)
    buffer->next_block++;

  return LAPLACE_STATUS_OK;
}

laplace_status_t laplace_buffer_deallocate(
    laplace_buffer_void_t *buffer, ptrdiff_t cell_size,
    laplace_handle_t handle) {
  if (handle.id < 0 || handle.id >= buffer->blocks.size ||
      buffer->blocks.values[handle.id].index == LAPLACE_ID_UNDEFINED)
    return LAPLACE_BUFFER_ERROR_INVALID_HANDLE_ID;

  if (handle.generation !=
      buffer->blocks.values[handle.id].generation)
    return LAPLACE_BUFFER_ERROR_INVALID_HANDLE_GENERATION;

  buffer_free(buffer, buffer->blocks.values[handle.id].index);

  buffer->blocks.values[handle.id].index = LAPLACE_ID_UNDEFINED;

  if (buffer->next_block > handle.id)
    buffer->next_block = handle.id;
  if (buffer->next_block < buffer->reserved)
    buffer->next_block = buffer->reserved;

  return LAPLACE_STATUS_OK;
}

laplace_status_t laplace_buffer_check(laplace_buffer_void_t *buffer,
                                      ptrdiff_t        cell_size,
                                      laplace_handle_t handle,
                                      ptrdiff_t const  index,
                                      ptrdiff_t const  size) {
  if (handle.id < 0 || handle.id >= buffer->blocks.size ||
      buffer->blocks.values[handle.id].index == LAPLACE_ID_UNDEFINED)
    return LAPLACE_BUFFER_ERROR_INVALID_HANDLE_ID;
  if (buffer->blocks.values[handle.id].generation !=
      handle.generation)
    return LAPLACE_BUFFER_ERROR_INVALID_HANDLE_GENERATION;
  if (size == 0)
    return LAPLACE_STATUS_OK;
  if (size < 0)
    return LAPLACE_BUFFER_ERROR_INVALID_SIZE;
  if (index < 0 ||
      index + size > buffer->blocks.values[handle.id].size)
    return LAPLACE_BUFFER_ERROR_INVALID_INDEX;
  return LAPLACE_STATUS_OK;
}
