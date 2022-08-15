#include "generator.h"

#include <string.h>

laplace_status_t laplace_generator_init(
    laplace_generator_t *generator, laplace_action_t action,
    laplace_read_only_t access) {
  memset(generator, 0, sizeof *generator);
  generator->promise = (laplace_promise_t *) action.alloc.allocate(
      action.alloc.state, action.size);
  if (generator->promise == NULL)
    return LAPLACE_GENERATOR_ERROR_BAD_ALLOC;
  generator->tick_duration = action.tick_duration;
  laplace_status_t s = action.starter(generator->promise, action.data,
                                      action.alloc, access);
  action.alloc.deallocate(action.alloc.state, action.data);
  return s;
}

void laplace_generator_destroy(laplace_generator_t generator) {
  generator.promise->alloc.deallocate(generator.promise->alloc.state,
                                      generator.promise);
}

laplace_impact_list_t laplace_generator_run(
    laplace_generator_t generator) {
  return AF_RESUME_AND_JOIN(*generator.promise);
}

laplace_generator_status_t laplace_generator_status(
    laplace_generator_t generator) {
  return AF_FINISHED(*generator.promise) ? LAPLACE_GENERATOR_FINISHED
                                         : LAPLACE_GENERATOR_RUNNING;
}
