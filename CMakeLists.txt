cmake_minimum_required(VERSION 3.16)

# Project version policy.
set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

# Allow to set up the options for the dependencies.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# Allow to specify the MSVC runtime library for the dependencies.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_BUILD_PARALLEL_LEVEL 3)
endif()

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/options.cmake)

project(
  ${LAPLACE_PROJECT}
    VERSION 0.1.1
    DESCRIPTION "A game engine focused on RTS."
    LANGUAGES CXX)

include(cmake/fetch-deps.cmake)

if(LAPLACE_ENABLE_OBJ OR LAPLACE_ENABLE_EXE OR LAPLACE_ENABLE_LIB)
  include(cmake/codegen.cmake)
  include(cmake/build.cmake)

  add_library(${LAPLACE_OBJ} OBJECT)
  add_library(${QUADWAR_OBJ} OBJECT)
  add_subdirectory(source)

  target_link_libraries(
    ${LAPLACE_OBJ}
      PUBLIC
        ${LAPLACE_BUILD}
        ${LAPLACE_DEPENDENCIES})
endif()

if(LAPLACE_ENABLE_EXE OR LAPLACE_ENABLE_LIB)
  target_link_libraries(
    ${QUADWAR_OBJ}
      PUBLIC
        ${LAPLACE_BUILD}
        ${LAPLACE_OBJ})

  if(LAPLACE_ENABLE_LIB)
    add_library(${LAPLACE_LIB} STATIC)

    target_include_directories(
      ${LAPLACE_LIB}
        PUBLIC
          "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

    target_link_libraries(
      ${LAPLACE_LIB}
        PUBLIC
          ${LAPLACE_BUILD}
          ${LAPLACE_OBJ})
  endif()

  if(LAPLACE_ENABLE_EXE OR LAPLACE_ENABLE_TESTING OR LAPLACE_ENABLE_COVERAGE)
    add_executable(${QUADWAR_EXE})

    target_link_libraries(
      ${QUADWAR_EXE}
        ${LAPLACE_BUILD}
        ${QUADWAR_OBJ}
        ${LAPLACE_OBJ})

    if(MSVC)
      target_link_options(${QUADWAR_EXE} PRIVATE "/SUBSYSTEM:CONSOLE")
    endif()
  endif()

  if(LAPLACE_ENABLE_TESTING OR LAPLACE_ENABLE_COVERAGE)
    enable_testing()

    macro(laplace_add_test name)
      add_test(
        NAME ${LAPLACE_UNITTESTS}-${name}
        COMMAND ${QUADWAR_EXE} --tests --gtest_filter=${name}.*)

      set_tests_properties(
        ${LAPLACE_UNITTESTS}-${name}
        PROPERTIES
        TIMEOUT "15")
    endmacro()

    laplace_add_test(core)
    laplace_add_test(math)
    laplace_add_test(engine)
    laplace_add_test(network)
    laplace_add_test(format)
    laplace_add_test(ui)
    laplace_add_test(platform)
    laplace_add_test(graphics)
    laplace_add_test(render)
    laplace_add_test(stem)
  endif()
endif()

include(cmake/package.cmake)
