cmake_minimum_required(VERSION 3.18)

set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

option(LAPLACE_ENABLE_EXE       "Enable executable" ON)
option(LAPLACE_ENABLE_TESTING   "Enable testing"    OFF)
option(LAPLACE_ENABLE_COVERAGE  "Enable coverage"   OFF)

set(LAPLACE_PROJECT   laplace)
set(LAPLACE_EXE       laplace)
set(LAPLACE_CONFIG    laplace-config)
set(LAPLACE_SOCKETS   laplace-sockets)
set(LAPLACE_UNITTESTS laplace-unittests)

set(CMAKE_CXX_STANDARD 20)

project(
  ${LAPLACE_PROJECT}
    VERSION 0.1.1
    DESCRIPTION "A game engine focused on RTS."
    LANGUAGES CXX
)

find_package(Python3 REQUIRED)

include(cmake/fetch-deps.cmake)

if(LAPLACE_ENABLE_EXE)
  include(cmake/config.cmake)

  execute_process(
    COMMAND ${Python3_EXECUTABLE} embed.py
    COMMAND ${Python3_EXECUTABLE} gen-gl.py
    WORKING_DIRECTORY tools
  )

  add_executable(${LAPLACE_EXE})
  add_subdirectory(source)

  target_compile_features(${LAPLACE_EXE} PRIVATE cxx_std_20)

  if(MSVC)
    target_link_options(${LAPLACE_EXE} PRIVATE "/SUBSYSTEM:CONSOLE")
  endif()

  target_link_libraries(
    ${LAPLACE_EXE}
      ${LAPLACE_CONFIG} gtest benchmark wolfssl bz2_static freetype
  )

  if(LAPLACE_ENABLE_TESTING OR LAPLACE_ENABLE_COVERAGE)
    enable_testing()

    add_test(
      NAME ${LAPLACE_UNITTESTS}
      COMMAND ${LAPLACE_EXE} --tests
    )
  endif()
endif()

include(GNUInstallDirs)

include(cmake/hotfix-ft2.cmake)
