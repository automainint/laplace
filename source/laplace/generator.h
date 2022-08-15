#ifndef LAPLACE_GENERATOR_H
#define LAPLACE_GENERATOR_H

#include "impact.h"

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  LAPLACE_GENERATOR_RUNNING  = 0,
  LAPLACE_GENERATOR_FINISHED = 1
} laplace_generator_status_t;

struct laplace_promise {
  KIT_AF_STATE_DATA;
  laplace_impact_list_t return_value;
  kit_allocator_t       alloc;
  laplace_read_only_t   access;
  laplace_handle_t      self;
};

typedef struct {
  laplace_promise_t *promise;
  laplace_time_t     tick_duration;
  laplace_time_t     clock;
} laplace_generator_t;

laplace_status_t laplace_generator_init(
    laplace_generator_t *generator, laplace_action_t action,
    laplace_read_only_t access, kit_allocator_t alloc);

void laplace_generator_destroy(laplace_generator_t generator);

laplace_impact_list_t laplace_generator_run(
    laplace_generator_t generator);

laplace_generator_status_t laplace_generator_status(
    laplace_generator_t generator);

#define LAPLACE_ACTION(coro_, tick_duration_, self_)   \
  { .size          = sizeof(AF_TYPE(coro_)),           \
    .coro          = AF_NAME(coro_),                   \
    .tick_duration = (tick_duration_),                 \
    .self          = (self_) };                                 \
  do {                                                 \
    assert(offsetof(AF_TYPE(coro_), return_value) ==   \
           offsetof(laplace_promise_t, return_value)); \
    assert(offsetof(AF_TYPE(coro_), alloc) ==          \
           offsetof(laplace_promise_t, alloc));        \
    assert(offsetof(AF_TYPE(coro_), access) ==         \
           offsetof(laplace_promise_t, access));       \
    assert(offsetof(AF_TYPE(coro_), self) ==           \
           offsetof(laplace_promise_t, self));         \
  } while (0)

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define GENERATOR_ERROR_BAD_ALLOC LAPLACE_GENERATOR_ERROR_BAD_ALLOC
#  define GENERATOR_RUNNING LAPLACE_GENERATOR_RUNNING
#  define GENERATOR_FINISHED LAPLACE_GENERATOR_FINISHED
#  define ACTION LAPLACE_ACTION

#  define generator_status_t laplace_generator_status_t
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
