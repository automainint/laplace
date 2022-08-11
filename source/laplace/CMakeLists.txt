target_sources(
  ${LAPLACE_LIBRARY}
    PRIVATE
      buffer.c impact.cpp io_impl.cpp entity.cpp options.cpp
      access.cpp execution.cpp controller.cpp x_buffer.cpp memory.cpp
      action.cpp state.cpp
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/execution.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/io_impl.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/action.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/access.impl.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_buffer.impl.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/buffer.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/operations.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/entity.impl.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/state.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/memory.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/access.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/impact.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/controller.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/impact.impl.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/x_buffer.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/entity.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/io_interface.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/options.h>)
