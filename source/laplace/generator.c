#include "generator.h"

#include <string.h>

laplace_status_t laplace_generator_init(
    laplace_generator_t *generator, laplace_action_t action,
    laplace_read_only_t access, kit_allocator_t alloc) {
  if (action.size > LAPLACE_COROUTINE_SIZE)
    return LAPLACE_ERROR_INVALID_SIZE;
  memset(generator, 0, sizeof *generator);
  AF_INIT_EXPLICIT(generator->promise, action.size, action.coro);
  generator->promise.alloc  = alloc;
  generator->promise.access = access;
  generator->promise.self   = action.self;
  generator->tick_duration  = action.tick_duration;
  return LAPLACE_STATUS_OK;
}

laplace_impact_list_t laplace_generator_run(
    laplace_generator_t *const generator) {
  AF_EXECUTE(&generator->promise);
  laplace_impact_list_t list = generator->promise.return_value;
  memset(&generator->promise.return_value, 0,
         sizeof generator->promise.return_value);
  return list;
}

laplace_generator_status_t laplace_generator_status(
    laplace_generator_t const *const generator) {
  return AF_FINISHED(generator->promise) ? LAPLACE_GENERATOR_FINISHED
                                         : LAPLACE_GENERATOR_RUNNING;
}
