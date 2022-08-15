target_sources(
  ${LAPLACE_TEST}
    PRIVATE
      main.test.c state.test.c generator.test.c buffer.test.c
      layout.test.c x_execution.test.cpp x_state.test.cpp x_action.test.cpp
      x_impact.test.cpp x_buffer.test.cpp help_thread.test.cpp x_access.test.cpp
      x_entity.test.cpp
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_io_impl.test.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/help_thread.test.h>)
