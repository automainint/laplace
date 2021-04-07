cmake_minimum_required(VERSION 3.18)

set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

option(LAPLACE_ENABLE_EXE       "Enable executable"     ON)
option(LAPLACE_ENABLE_LIB       "Enable static library" OFF)
option(LAPLACE_ENABLE_TESTING   "Enable testing"        OFF)
option(LAPLACE_ENABLE_COVERAGE  "Enable coverage"       OFF)

set(LAPLACE_PROJECT   laplace)
set(LAPLACE_OBJ       laplace-obj)
set(LAPLACE_LIB       laplace)
set(LAPLACE_CONFIG    laplace-config)
set(LAPLACE_SOCKETS   laplace-sockets)
set(LAPLACE_UNITTESTS laplace-unittests)
set(QUADWAR_EXE       quadwar)

set(CMAKE_CXX_STANDARD 20)

project(
  ${LAPLACE_PROJECT}
    VERSION 0.1.1
    DESCRIPTION "A game engine focused on RTS."
    LANGUAGES CXX
)

find_package(Python3 REQUIRED)

include(cmake/fetch-deps.cmake)

if(LAPLACE_ENABLE_EXE OR LAPLACE_ENABLE_LIB)
  execute_process(
    COMMAND ${Python3_EXECUTABLE} embed.py
    COMMAND ${Python3_EXECUTABLE} gen-gl.py
    WORKING_DIRECTORY tools
  )

  include(cmake/config.cmake)

  add_library(${LAPLACE_OBJ} OBJECT)

  target_link_libraries(
    ${LAPLACE_OBJ}
      PUBLIC
        ${LAPLACE_CONFIG}
        gtest benchmark wolfssl bz2_static freetype
  )

  target_compile_features(${LAPLACE_OBJ} PRIVATE cxx_std_20)

  if(LAPLACE_ENABLE_LIB)
    add_library(${LAPLACE_LIB} STATIC)
    target_link_libraries(${LAPLACE_LIB} ${LAPLACE_OBJ})
  endif()

  if(LAPLACE_ENABLE_EXE)
    add_executable(${QUADWAR_EXE})
    target_link_libraries(${QUADWAR_EXE} ${LAPLACE_OBJ})

    target_compile_features(${QUADWAR_EXE} PRIVATE cxx_std_20)

    if(MSVC)
      target_link_options(${QUADWAR_EXE} PRIVATE "/SUBSYSTEM:CONSOLE")
    endif()

    if(LAPLACE_ENABLE_TESTING OR LAPLACE_ENABLE_COVERAGE)
      enable_testing()

      add_test(
        NAME ${LAPLACE_UNITTESTS}
        COMMAND ${QUADWAR_EXE} --tests
      )
    endif()
  endif()
endif()

add_subdirectory(source)

include(GNUInstallDirs)
include(cmake/hotfix-ft2.cmake)
