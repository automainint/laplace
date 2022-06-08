target_sources(
  ${LAPLACE_LIBRARY}
    PRIVATE
      impact.cpp buffer.cpp entity.cpp execution.cpp
      controller.cpp
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/execution.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/buffer.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/impact.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/controller.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/entity.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/buffer.impl.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/options.h>)
