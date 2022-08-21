target_sources(
  ${LAPLACE_TEST}
    PRIVATE
      execution.test.c main.test.c state.test.c impact.test.c
      generator.test.c buffer.test.c layout.test.c
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_io_impl.test.h>)
