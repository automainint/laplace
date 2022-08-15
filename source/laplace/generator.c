#include "generator.h"

#include <string.h>

laplace_status_t laplace_generator_init(
    laplace_generator_t *generator, laplace_action_t action,
    laplace_read_only_t access, kit_allocator_t alloc) {
  memset(generator, 0, sizeof *generator);
  generator->promise = (laplace_promise_t *) alloc.allocate(
      alloc.state, action.size);
  if (generator->promise == NULL)
    return LAPLACE_GENERATOR_ERROR_BAD_ALLOC;
  laplace_status_t s = action.starter(generator->promise, alloc,
                                      action.self, access);
  if (s != LAPLACE_STATUS_OK)
    alloc.deallocate(alloc.state, generator->promise);
  generator->tick_duration = action.tick_duration;
  return s;
}

void laplace_generator_destroy(laplace_generator_t generator) {
  generator.promise->alloc.deallocate(generator.promise->alloc.state,
                                      generator.promise);
}

laplace_impact_list_t laplace_generator_run(
    laplace_generator_t generator) {
  AF_EXECUTE(generator.promise);
  return generator.promise->return_value;
}

laplace_generator_status_t laplace_generator_status(
    laplace_generator_t generator) {
  return AF_FINISHED(*generator.promise) ? LAPLACE_GENERATOR_FINISHED
                                         : LAPLACE_GENERATOR_RUNNING;
}
