target_sources(
  ${LAPLACE_LIBRARY}
    PRIVATE
      handle.c impact.c access.c buffer.c action.c execution.c
      generator.c layout.c state.c x_execution.cpp x_options.cpp
      x_entity.cpp x_access.cpp x_io_impl.cpp x_impact.cpp x_action.cpp
      x_memory.cpp x_buffer.cpp x_state.cpp
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/handle.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_io_interface.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/generator.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/execution.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_state.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_operations.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/action.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_access.impl.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_access.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_impact.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_action.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_io_impl.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_buffer.impl.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_entity.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/buffer.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_entity.impl.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_impact.impl.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/state.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_memory.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/layout.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/access.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/impact.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_execution.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_buffer.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/options.h>)
