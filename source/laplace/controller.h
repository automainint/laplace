#ifndef LAPLACE_CONTROLLER_H
#define LAPLACE_CONTROLLER_H

#include <kit/dynamic_array.h>

#include "action.h"
#include "execution.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  laplace_time_t   time;
  laplace_action_t action;
} laplace_event_t;

typedef struct {
  laplace_time_t time;
  KIT_DA(history, laplace_event_t);
} laplace_controller_t;

kit_status_t laplace_controller_init(laplace_controller_t *controller,
                                     ptrdiff_t reserved_history_size,
                                     kit_allocator_t alloc);

kit_status_t laplace_controller_destroy(
    laplace_controller_t *controller);

kit_status_t laplace_controller_queue(
    laplace_controller_t *controller, laplace_event_t event);

kit_status_t laplace_controller_rewind(
    laplace_controller_t *controller, laplace_execution_t *execution,
    laplace_time_t time);

kit_status_t laplace_schedule(laplace_controller_t *controller,
                              laplace_execution_t  *execution,
                              laplace_time_t        time_elapsed);

kit_status_t laplace_schedule_and_join(
    laplace_controller_t *controller, laplace_execution_t *execution,
    laplace_time_t time_elapsed);

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define event_t laplace_event_t
#  define controller_t laplace_controller_t

#  define controller_init laplace_controller_init
#  define controller_destroy laplace_controller_destroy
#  define controller_queue laplace_controller_queue
#  define schedule laplace_schedule
#  define shedule_and_join laplace_schedule_and_join
#endif

#ifdef __cplusplus
}
#endif

#endif

