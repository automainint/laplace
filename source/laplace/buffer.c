#include "buffer.h"

/*  TODO
 *  buffer_realloc
 */

#define LOAD_ atomic_load_explicit
#define STORE_ atomic_store_explicit
#define ADD_ atomic_fetch_add_explicit

#define RLX_ memory_order_relaxed
#define ACQ_ memory_order_acquire
#define RLS_ memory_order_release

static ptrdiff_t buffer_alloc(laplace_buffer_void_t *const buffer,
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
    int success = 1;

    if (mtx_lock(&buffer->read_lock) == thrd_success) {
      while (success && buffer->read_count != 0)
        if (cnd_wait(&buffer->read_sync, &buffer->read_lock) !=
            thrd_success) {
          (void) mtx_unlock(&buffer->read_lock);
          success = 0;
        }
    } else
      success = 0;

    if (success) {
      DA_RESIZE(buffer->info, offset + size);

      if (buffer->info.size != offset + size)
        success = 0;

      da_resize((da_void_t *) &buffer->data, buffer->cell_size,
                offset + size);

      if (buffer->data.size != offset + size)
        success = 0;

      (void) mtx_unlock(&buffer->read_lock);
    }

    if (!success)
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
    assert(end >= 0 && end < LAPLACE_BUF_INFO_(*buffer).size);
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
    return LAPLACE_ERROR_INVALID_BUFFER;
  if (chunk_size <= 0)
    return LAPLACE_ERROR_INVALID_CHUNK_SIZE;

  buffer->chunk_size = chunk_size;
  return STATUS_OK;
}

laplace_handle_t laplace_buffer_allocate(
    laplace_buffer_void_t *const buffer, ptrdiff_t const size) {
  laplace_handle_t h;

  if (size <= 0) {
    h.id = LAPLACE_ID_UNDEFINED, h.error = LAPLACE_ERROR_INVALID_SIZE;
    return h;
  }

  ptrdiff_t const offset = buffer_alloc(buffer, size);

  if (offset == LAPLACE_ID_UNDEFINED) {
    h.id    = LAPLACE_ID_UNDEFINED;
    h.error = LAPLACE_ERROR_BAD_ALLOC;
    return h;
  }

  if (mtx_lock(&buffer->read_lock) == thrd_success) {
    while (buffer->read_count != 0)
      if (cnd_wait(&buffer->read_sync, &buffer->read_lock) !=
          thrd_success) {
        (void) mtx_unlock(&buffer->read_lock);
        h.id    = LAPLACE_ID_UNDEFINED;
        h.error = LAPLACE_ERROR_BAD_CNDVAR_WAIT;
        return h;
      }
  } else {
    h.id    = LAPLACE_ID_UNDEFINED;
    h.error = LAPLACE_ERROR_BAD_MUTEX_LOCK;
    return h;
  }

  ptrdiff_t const block = buffer->next_block;
  if (block >= buffer->blocks.size) {
    ptrdiff_t i = buffer->blocks.size;

    DA_RESIZE(buffer->blocks, block + 1);

    if (buffer->blocks.size != block + 1) {
      buffer_free(buffer, offset);
      (void) mtx_unlock(&buffer->read_lock);
      h.id    = LAPLACE_ID_UNDEFINED;
      h.error = LAPLACE_ERROR_BAD_ALLOC;
      return h;
    }

    for (; i < buffer->blocks.size; i++) {
      assert(i >= 0 && i < buffer->blocks.size);
      STORE_(&buffer->blocks.values[i].index, LAPLACE_ID_UNDEFINED,
             RLS_);
      STORE_(&buffer->blocks.values[i].generation, -1, RLS_);
      STORE_(&buffer->blocks.values[i].size, 0, RLS_);
    }
  }

  assert(block >= 0 && block < buffer->blocks.size);
  STORE_(&buffer->blocks.values[block].index, offset, RLS_);

  (void) mtx_unlock(&buffer->read_lock);

  ptrdiff_t const generation = ADD_(
      &buffer->blocks.values[block].generation, 1, RLS_);
  STORE_(&buffer->blocks.values[block].size, size, RLS_);

  assert(buffer->next_block >= 0 &&
         buffer->next_block <= buffer->blocks.size);
  while (buffer->next_block < buffer->blocks.size &&
         LOAD_(&buffer->blocks.values[buffer->next_block].index,
               RLX_) != LAPLACE_ID_UNDEFINED)
    buffer->next_block++;

  h.id         = block;
  h.generation = generation + 1;
  return h;
}

laplace_handle_t laplace_buffer_allocate_into(
    laplace_buffer_void_t *const buffer, ptrdiff_t const size,
    laplace_handle_t handle) {
  laplace_handle_t h;

  if (handle.id < 0) {
    h.id    = LAPLACE_ID_UNDEFINED;
    h.error = LAPLACE_ERROR_INVALID_HANDLE_ID;
    return h;
  }

  if (handle.id >= buffer->blocks.size && handle.generation != -1) {
    h.id    = LAPLACE_ID_UNDEFINED;
    h.error = LAPLACE_ERROR_INVALID_HANDLE_GENERATION;
    return h;
  }

  ptrdiff_t generation = -1;

  if (handle.id < buffer->blocks.size) {
    generation = LOAD_(&buffer->blocks.values[handle.id].generation,
                       RLX_);

    if (generation != handle.generation) {
      h.id    = LAPLACE_ID_UNDEFINED;
      h.error = LAPLACE_ERROR_INVALID_HANDLE_GENERATION;
      return h;
    }
  }

  if (size <= 0) {
    h.id    = LAPLACE_ID_UNDEFINED;
    h.error = LAPLACE_ERROR_INVALID_SIZE;
    return h;
  }

  if (handle.id < buffer->blocks.size) {
    ptrdiff_t const block = LOAD_(
        &buffer->blocks.values[handle.id].index, RLX_);

    if (block != LAPLACE_ID_UNDEFINED)
      buffer_free(buffer, block);
  }

  ptrdiff_t const offset = buffer_alloc(buffer, size);

  if (offset == LAPLACE_ID_UNDEFINED) {
    h.id    = LAPLACE_ID_UNDEFINED;
    h.error = LAPLACE_ERROR_BAD_ALLOC;
    return h;
  }

  if (mtx_lock(&buffer->read_lock) == thrd_success) {
    while (buffer->read_count != 0)
      if (cnd_wait(&buffer->read_sync, &buffer->read_lock) !=
          thrd_success) {
        (void) mtx_unlock(&buffer->read_lock);
        h.id    = LAPLACE_ID_UNDEFINED;
        h.error = LAPLACE_ERROR_BAD_CNDVAR_WAIT;
        return h;
      }
  } else {
    h.id    = LAPLACE_ID_UNDEFINED;
    h.error = LAPLACE_ERROR_BAD_MUTEX_LOCK;
    return h;
  }

  if (handle.id >= buffer->blocks.size) {
    ptrdiff_t i = buffer->blocks.size;

    DA_RESIZE(buffer->blocks, handle.id + 1);

    if (buffer->blocks.size != handle.id + 1) {
      buffer_free(buffer, offset);
      (void) mtx_unlock(&buffer->read_lock);
      h.id    = LAPLACE_ID_UNDEFINED;
      h.error = LAPLACE_ERROR_BAD_ALLOC;
      return h;
    }

    for (; i < buffer->blocks.size; i++) {
      assert(i >= 0 && i < buffer->blocks.size);
      STORE_(&buffer->blocks.values[i].index, LAPLACE_ID_UNDEFINED,
             RLS_);
      STORE_(&buffer->blocks.values[i].generation, -1, RLS_);
      STORE_(&buffer->blocks.values[i].size, 0, RLS_);
    }
  }

  STORE_(&buffer->blocks.values[handle.id].index, offset, RLS_);

  (void) mtx_unlock(&buffer->read_lock);

  STORE_(&buffer->blocks.values[handle.id].generation, generation + 1,
         RLS_);
  STORE_(&buffer->blocks.values[handle.id].size, size, RLS_);

  assert(buffer->next_block >= 0 &&
         buffer->next_block <= buffer->blocks.size);
  while (buffer->next_block < buffer->blocks.size &&
         LOAD_(&buffer->blocks.values[buffer->next_block].index,
               RLX_) != LAPLACE_ID_UNDEFINED)
    buffer->next_block++;

  h.id         = handle.id;
  h.generation = generation + 1;
  return h;
}

laplace_buffer_realloc_result_t laplace_buffer_reallocate(
    laplace_buffer_void_t *buffer, ptrdiff_t size,
    laplace_handle_t handle) {
  laplace_buffer_realloc_result_t result;
  memset(&result, 0, sizeof result);

  if (size <= 0) {
    result.status = LAPLACE_ERROR_INVALID_SIZE;
    return result;
  }

  if (handle.id < 0 || handle.id >= buffer->blocks.size) {
    result.status = LAPLACE_ERROR_INVALID_HANDLE_ID;
    return result;
  }

  if (handle.generation !=
      LOAD_(&buffer->blocks.values[handle.id].generation, RLX_)) {
    result.status = LAPLACE_ERROR_INVALID_HANDLE_GENERATION;
    return result;
  }

  ptrdiff_t const offset = buffer_alloc(buffer, size);

  if (offset == LAPLACE_ID_UNDEFINED) {
    result.status = LAPLACE_ERROR_BAD_ALLOC;
    return result;
  }

  result.offset          = offset;
  result.previous_offset = LOAD_(
      &buffer->blocks.values[handle.id].index, RLX_);
  result.previous_size = LOAD_(&buffer->blocks.values[handle.id].size,
                               RLX_);

  buffer_free(buffer, result.previous_offset);

  if (mtx_lock(&buffer->read_lock) == thrd_success) {
    while (buffer->read_count != 0)
      if (cnd_wait(&buffer->read_sync, &buffer->read_lock) !=
          thrd_success) {
        (void) mtx_unlock(&buffer->read_lock);
        result.status = LAPLACE_ERROR_BAD_CNDVAR_WAIT;
        return result;
      }
  } else {
    result.status = LAPLACE_ERROR_BAD_MUTEX_LOCK;
    return result;
  }

  STORE_(&buffer->blocks.values[handle.id].index, offset, RLS_);

  (void) mtx_unlock(&buffer->read_lock);

  STORE_(&buffer->blocks.values[handle.id].size, size, RLS_);

  result.status = LAPLACE_STATUS_OK;
  return result;
}

laplace_status_t laplace_buffer_reserve(
    laplace_buffer_void_t *const buffer, ptrdiff_t const size) {
  if (size < 0)
    return LAPLACE_ERROR_INVALID_SIZE;

  if (buffer->blocks.size < size) {
    ptrdiff_t i = buffer->blocks.size;

    if (mtx_lock(&buffer->read_lock) == thrd_success) {
      while (buffer->read_count != 0)
        if (cnd_wait(&buffer->read_sync, &buffer->read_lock) !=
            thrd_success) {
          (void) mtx_unlock(&buffer->read_lock);
          return LAPLACE_ERROR_BAD_CNDVAR_WAIT;
        }
    } else
      return LAPLACE_ERROR_BAD_MUTEX_LOCK;

    DA_RESIZE(buffer->blocks, size);

    if (buffer->blocks.size != size) {
      (void) mtx_unlock(&buffer->read_lock);
      return LAPLACE_ERROR_BAD_ALLOC;
    }

    for (; i < buffer->blocks.size; i++) {
      assert(i >= 0 && i < buffer->blocks.size);
      STORE_(&buffer->blocks.values[i].index, LAPLACE_ID_UNDEFINED,
             RLS_);
      STORE_(&buffer->blocks.values[i].generation, -1, RLS_);
    }

    (void) mtx_unlock(&buffer->read_lock);
  }

  buffer->reserved = size;

  if (buffer->next_block < size)
    buffer->next_block = size;

  assert(buffer->next_block >= 0 &&
         buffer->next_block <= buffer->blocks.size);
  while (buffer->next_block < buffer->blocks.size &&
         LOAD_(&buffer->blocks.values[buffer->next_block].index,
               RLX_) == LAPLACE_ID_UNDEFINED)
    buffer->next_block++;

  return LAPLACE_STATUS_OK;
}

laplace_status_t laplace_buffer_deallocate(
    laplace_buffer_void_t *buffer, laplace_handle_t handle) {
  if (handle.id < 0 || handle.id >= buffer->blocks.size)
    return LAPLACE_ERROR_INVALID_HANDLE_ID;

  ptrdiff_t const block = LOAD_(
      &buffer->blocks.values[handle.id].index, RLX_);

  if (block == LAPLACE_ID_UNDEFINED)
    return LAPLACE_ERROR_INVALID_HANDLE_ID;

  if (handle.generation !=
      LOAD_(&buffer->blocks.values[handle.id].generation, RLX_))
    return LAPLACE_ERROR_INVALID_HANDLE_GENERATION;

  buffer_free(buffer, block);

  STORE_(&buffer->blocks.values[handle.id].index,
         LAPLACE_ID_UNDEFINED, RLS_);

  if (buffer->next_block > handle.id)
    buffer->next_block = handle.id;
  if (buffer->next_block < buffer->reserved)
    buffer->next_block = buffer->reserved;

  return LAPLACE_STATUS_OK;
}

laplace_status_t laplace_buffer_check(
    laplace_buffer_void_t const *buffer, laplace_handle_t handle,
    ptrdiff_t const index, ptrdiff_t const size) {
  if (handle.id < 0 || handle.id >= buffer->blocks.size)
    return LAPLACE_ERROR_INVALID_HANDLE_ID;
  ptrdiff_t const block = LOAD_(
      &buffer->blocks.values[handle.id].index, ACQ_);
  ptrdiff_t const generation = LOAD_(
      &buffer->blocks.values[handle.id].generation, ACQ_);
  if (block == LAPLACE_ID_UNDEFINED)
    return LAPLACE_ERROR_INVALID_HANDLE_ID;
  if (generation != handle.generation)
    return LAPLACE_ERROR_INVALID_HANDLE_GENERATION;
  if (size == 0)
    return LAPLACE_STATUS_OK;
  if (size < 0)
    return LAPLACE_ERROR_INVALID_SIZE;
  if (index < 0 || block + index < 0 ||
      block + index + size > buffer->data.size ||
      index + size >
          LOAD_(&buffer->blocks.values[handle.id].size, ACQ_))
    return LAPLACE_ERROR_INVALID_INDEX;

  return LAPLACE_STATUS_OK;
}

ptrdiff_t laplace_buffer_size(laplace_buffer_void_t *buffer,
                              laplace_handle_t       handle) {
  if (mtx_lock(&buffer->read_lock) != thrd_success) {
    return 0;
  } else {
    buffer->read_count++;
    (void) mtx_unlock(&buffer->read_lock);
  }

  ptrdiff_t size = 0;
  if (laplace_buffer_check(buffer, handle, 0, 0) == LAPLACE_STATUS_OK)
    size = LOAD_(&buffer->blocks.values[handle.id].size, ACQ_);

  if (mtx_lock(&buffer->read_lock) != thrd_success) {
    assert(0);
    return 0;
  } else {
    ptrdiff_t const count = --buffer->read_count;
    (void) mtx_unlock(&buffer->read_lock);
    if (count == 0)
      (void) cnd_broadcast(&buffer->read_sync);
  }

  return size;
}
