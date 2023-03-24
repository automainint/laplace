cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)

if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_BUILD_PARALLEL_LEVEL 4 CACHE STRING "" FORCE)
endif()

option(LAPLACE_ENABLE_STATIC_DISPATCH "Enable static dispatch"  OFF)
option(LAPLACE_ENABLE_TESTING         "Enable testing"          ON)

project(
  laplace
  VERSION 0.1.1
  DESCRIPTION "Deterministic lockstep framework"
  LANGUAGES C)

include(FetchContent)

find_package(kit QUIET)

if(NOT kit_FOUND)
  FetchContent_Declare(
    kit
    GIT_REPOSITORY https://github.com/automainint/kit.git
    GIT_TAG dev)
  set(KIT_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(kit)
endif()

add_library(laplace STATIC)
add_library(laplace::laplace ALIAS laplace)
target_include_directories(
  laplace INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>)
target_compile_features(laplace PUBLIC c_std_11)
target_link_libraries(laplace PUBLIC kit::kit)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(laplace PUBLIC -pedantic -Wall -Werror)
endif()

if(LAPLACE_ENABLE_STATIC_DISPATCH)
  target_compile_definitions(laplace PUBLIC LAPLACE_ENABLE_STATIC_DISPATCH)
endif()

enable_testing()

if(LAPLACE_ENABLE_TESTING)
  add_executable(laplace_test_suite)
  add_executable(laplace::laplace_test_suite ALIAS laplace_test_suite)
  target_compile_features(laplace_test_suite PRIVATE c_std_11)
  target_link_libraries(laplace_test_suite PRIVATE laplace kit::kit_test)

  if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      laplace PUBLIC
      -fsanitize=undefined,address
      --coverage -O0 -g)
    target_link_options(
      laplace PUBLIC
      -fsanitize=undefined,address
      --coverage)
  endif()

  add_test(
    NAME laplace_unittests
    COMMAND laplace_test_suite --no-carriage-return)

  set_tests_properties(
    laplace_unittests
    PROPERTIES
    TIMEOUT "120")
endif()

add_subdirectory(source)

include(GNUInstallDirs)

file(
  GLOB_RECURSE headers_
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source/laplace
  ${CMAKE_CURRENT_SOURCE_DIR}/source/laplace/*.h)

foreach(path_ ${headers_})
  get_filename_component(dir_ "${path_}" DIRECTORY)

  install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/laplace/${path_}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/laplace/${dir_})
endforeach()

unset(headers_)
unset(path_)
unset(dir_)

install(
  TARGETS laplace
  EXPORT laplace-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
  EXPORT laplace-targets
  FILE laplace-targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

export(
  EXPORT laplace-targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/laplace-targets.cmake"
  NAMESPACE ${PROJECT_NAME}::)

include(CMakePackageConfigHelpers)

string(TOLOWER ${PROJECT_NAME} project_lower_)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config-version.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion)

install(
  FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${project_lower_}-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

unset(project_lower_)
