target_sources(
  ${LAPLACE_TEST}
    PRIVATE
      entity.test.cpp impact.test.cpp buffer.test.cpp
      state.test.cpp controller.test.cpp execution.test.cpp main.test.cpp
      access.test.cpp action.test.cpp
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/io_impl.test.h>)
