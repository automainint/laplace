#ifndef LAPLACE_GENERATOR_H
#define LAPLACE_GENERATOR_H

#include "access.h"
#include "impact.h"

#ifdef __cplusplus
extern "C" {
#endif

enum { LAPLACE_GENERATOR_ERROR_BAD_ALLOC = 1 };

typedef enum {
  LAPLACE_GENERATOR_RUNNING  = 0,
  LAPLACE_GENERATOR_FINISHED = 1
} laplace_generator_status_t;

typedef struct {
  KIT_AF_STATE_DATA;
  laplace_impact_list_t return_value;
  kit_allocator_t       alloc;
  laplace_read_only_t   access;
} laplace_promise_t;

typedef struct {
  laplace_promise_t *promise;
  laplace_time_t     tick_duration;
  laplace_time_t     clock;
} laplace_generator_t;

laplace_status_t laplace_generator_init(
    laplace_generator_t *generator, laplace_action_t action,
    laplace_read_only_t access);

void laplace_generator_destroy(laplace_generator_t generator);

laplace_impact_list_t laplace_generator_run(
    laplace_generator_t generator);

laplace_generator_status_t laplace_generator_status(
    laplace_generator_t generator);

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define GENERATOR_ERROR_BAD_ALLOC LAPLACE_GENERATOR_ERROR_BAD_ALLOC
#  define GENERATOR_RUNNING LAPLACE_GENERATOR_RUNNING
#  define GENERATOR_FINISHED LAPLACE_GENERATOR_FINISHED

#  define generator_status_t laplace_generator_status_t
#  define promise_t laplace_promise_t
#  define generator_t laplace_generator_t

#  define generator_init laplace_generator_init
#  define generator_destroy laplace_generator_destroy
#  define generator_run laplace_generator_run
#  define generator_status laplace_generator_status
#endif

#ifdef __cplusplus
}
#endif

#endif
