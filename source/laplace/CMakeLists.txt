target_sources(
  ${LAPLACE_LIBRARY}
    PRIVATE
      handle.c impact.c access.c buffer.c action.c execution.c
      generator.c controller.c layout.c state.c
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/handle.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/generator.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/execution.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/action.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/buffer.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/state.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/layout.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/access.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/impact.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/controller.h>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/options.h>)
