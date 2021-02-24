cmake_minimum_required(VERSION 3.18)

set(PROJECT_NAME laplace-project)
set(EXE_NAME laplace)

project(${PROJECT_NAME} CXX)

find_package(Threads REQUIRED)

add_executable(${EXE_NAME})

add_library(Sockets INTERFACE)

set_property(
  TARGET ${EXE_NAME} PROPERTY CXX_STANDARD 20
)

if(MSVC)
  set_property(
    TARGET ${EXE_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded"
  )

  set_target_properties(
    ${EXE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE"
  )
endif()

if(WIN32)
  add_compile_definitions(_CONSOLE)

  target_link_libraries(
    Sockets INTERFACE ws2_32
  )
endif()

add_subdirectory(source)

target_link_libraries(
  ${EXE_NAME}
    Threads::Threads Sockets gtest freetype wolfssl
)

target_link_directories(
  ${EXE_NAME}
  PUBLIC lib
)

target_include_directories(
  ${EXE_NAME}
  PUBLIC include
)
