cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)

if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_BUILD_PARALLEL_LEVEL 4 CACHE STRING "" FORCE)
endif()

option(LAPLACE_ENABLE_TESTING "Enable testing" ON)

set(LAPLACE_PROJECT laplace)
set(LAPLACE_LIBRARY laplace)
set(LAPLACE_CODEGEN laplace_codegen)
set(LAPLACE_TEST laplace_test)
set(LAPLACE_TARGETS laplace-targets)

project(
  ${LAPLACE_PROJECT}
  VERSION 0.1.1
  DESCRIPTION "Laplace"
  LANGUAGES C)

include(FetchContent)

find_package(Threads)
find_package(kit QUIET)

if(NOT kit_FOUND)
  FetchContent_Declare(
    kit
    GIT_REPOSITORY https://github.com/automainint/kit.git
    GIT_TAG dev)
  set(KIT_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(kit)
endif()

add_library(${LAPLACE_LIBRARY} STATIC)
add_library(${LAPLACE_PROJECT}::${LAPLACE_LIBRARY} ALIAS ${LAPLACE_LIBRARY})
target_include_directories(
  ${LAPLACE_LIBRARY} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>)
target_compile_features(${LAPLACE_LIBRARY} PUBLIC c_std_11)
target_link_libraries(${LAPLACE_LIBRARY} PUBLIC Threads::Threads kit::kit)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(${LAPLACE_LIBRARY} PUBLIC -pedantic -Wall -Werror)
endif()

add_executable(${LAPLACE_CODEGEN})
add_executable(${LAPLACE_PROJECT}::${LAPLACE_CODEGEN} ALIAS ${LAPLACE_CODEGEN})
target_compile_features(${LAPLACE_CODEGEN} PRIVATE c_std_11)
target_link_libraries(${LAPLACE_CODEGEN} PRIVATE ${LAPLACE_LIBRARY})

enable_testing()

if(LAPLACE_ENABLE_TESTING)
  find_package(Threads REQUIRED)

  add_executable(${LAPLACE_TEST})
  add_executable(${LAPLACE_PROJECT}::${LAPLACE_TEST} ALIAS ${LAPLACE_TEST})
  target_compile_features(${LAPLACE_TEST} PRIVATE c_std_11)
  target_link_libraries(${LAPLACE_TEST} PRIVATE ${LAPLACE_LIBRARY} kit::kit_test Threads::Threads)

  if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      ${LAPLACE_LIBRARY} PUBLIC
      -fsanitize=undefined,address
      --coverage -O0 -g)
    target_link_options(
      ${LAPLACE_LIBRARY} PUBLIC
      -fsanitize=undefined,address
      --coverage)
  endif()

  add_test(
    NAME ${LAPLACE_LIBRARY}_unittests
    COMMAND ${LAPLACE_TEST} --no-carriage-return)

  set_tests_properties(
    ${LAPLACE_LIBRARY}_unittests
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
  TARGETS ${LAPLACE_LIBRARY} ${LAPLACE_TEST_LIBRARY}
  EXPORT ${LAPLACE_TARGETS}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
  EXPORT ${LAPLACE_TARGETS}
  FILE ${LAPLACE_TARGETS}.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

export(
  EXPORT ${LAPLACE_TARGETS}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${LAPLACE_TARGETS}.cmake"
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
