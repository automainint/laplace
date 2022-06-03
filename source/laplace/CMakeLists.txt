target_sources(
  ${LAPLACE_LIBRARY}
    PRIVATE
      execution.cpp
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/execution.h>)
