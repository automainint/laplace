#include "controller.h"

kit_status_t controller_init(controller_t *const controller,
                             ptrdiff_t const reserved_history_size,
                             kit_allocator_t const alloc) {
  if (reserved_history_size < 0)
    return ERROR_INVALID_SIZE;

  memset(controller, 0, sizeof *controller);

  DA_INIT(controller->history, reserved_history_size, alloc);

  if (controller->history.size != reserved_history_size)
    return ERROR_BAD_ALLOC;

  DA_RESIZE(controller->history, 0);

  return KIT_OK;
}

kit_status_t controller_destroy(controller_t *const controller) {
  DA_DESTROY(controller->history);

  return KIT_OK;
}

kit_status_t controller_queue(controller_t *const controller,
                              event_t const       event) {
  if (event.time < controller->time)
    return ERROR_INVALID_EVENT_TIME;

  ptrdiff_t const size     = controller->history.size;
  ptrdiff_t       position = size;

  while (position > 0 &&
         controller->history.values[position - 1].time > event.time)
    position--;

  DA_RESIZE(controller->history, size + 1);

  if (controller->history.size != size + 1)
    return ERROR_BAD_ALLOC;

  for (ptrdiff_t i = size; i > position; i--)
    memcpy(controller->history.values + i,
           controller->history.values + (i - 1),
           sizeof *controller->history.values);

  memcpy(controller->history.values + position, &event, sizeof event);

  return KIT_OK;
}

kit_status_t controller_rewind(controller_t *const  controller,
                               execution_t *const   execution,
                               laplace_time_t const time) {
  if (time < 0)
    return ERROR_INVALID_EVENT_TIME;

  return ERROR_NOT_IMPLEMENTED;
}

kit_status_t schedule(controller_t *const  controller,
                      execution_t *const   execution,
                      laplace_time_t const time_elapsed) {
  return ERROR_NOT_IMPLEMENTED;
}

kit_status_t schedule_and_join(controller_t *const  controller,
                               execution_t *const   execution,
                               laplace_time_t const time_elapsed) {
  return ERROR_NOT_IMPLEMENTED;
}

