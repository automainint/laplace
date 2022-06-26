cmake_minimum_required(VERSION 3.16)

option(LAPLACE_ENABLE_TESTING "Enable testing" ON)

set(LAPLACE_PROJECT laplace)
set(LAPLACE_LIBRARY laplace)
set(LAPLACE_TEST laplace-test)
set(LAPLACE_UNITTESTS laplace-unittests)
set(LAPLACE_TARGETS laplace-targets)

if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
  set(CMAKE_BUILD_PARALLEL_LEVEL 3)
endif()

project(
  ${LAPLACE_PROJECT}
  VERSION 0.1.1
  DESCRIPTION "Deterministic lockstep framework"
  LANGUAGES CXX)

add_library(${LAPLACE_LIBRARY} STATIC)
add_library(${LAPLACE_PROJECT}::${LAPLACE_LIBRARY} ALIAS ${LAPLACE_LIBRARY})
target_include_directories(
  ${LAPLACE_LIBRARY} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>)
target_compile_features(${LAPLACE_LIBRARY} PUBLIC cxx_std_20)

include(FetchContent)

find_package(Threads REQUIRED)
find_package(coro QUIET)

if(NOT coro_FOUND)
  FetchContent_Declare(
    coro
    GIT_REPOSITORY https://github.com/automainint/coro.git
    GIT_TAG dev)

  set(CORO_ENABLE_TESTING OFF)

  FetchContent_MakeAvailable(coro)
endif()

if(LAPLACE_ENABLE_TESTING)
  find_package(Catch2 QUIET)
  
  if(NOT Catch2_FOUND)
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v2.13.9)

    FetchContent_MakeAvailable(Catch2)
  endif()
endif()

target_link_libraries(${LAPLACE_LIBRARY} PUBLIC Threads::Threads coro::coro)

enable_testing()

if(LAPLACE_ENABLE_TESTING)
  add_executable(${LAPLACE_TEST})

  target_link_libraries(
    ${LAPLACE_TEST}
    PRIVATE
    ${LAPLACE_LIBRARY} Catch2::Catch2)

  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      ${LAPLACE_LIBRARY} PUBLIC
      -fsanitize=undefined#,address
      --coverage -O0 -g)
    target_link_options(
      ${LAPLACE_LIBRARY} PUBLIC
      -fsanitize=undefined#,address
      --coverage)
  endif()

  add_test(
    NAME ${LAPLACE_UNITTESTS}
    COMMAND ${LAPLACE_TEST})

  set_tests_properties(
    ${LAPLACE_UNITTESTS}
    PROPERTIES
    TIMEOUT "30")
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
  TARGETS ${LAPLACE_LIBRARY}
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
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in
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
