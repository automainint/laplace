cmake_minimum_required(VERSION 3.16)
set(NAME laplace-integration-fetch-content)
project(${NAME} CXX)

include(FetchContent)
FetchContent_Declare(
  laplace
  GIT_REPOSITORY https://github.com/automainint/laplace.git
  GIT_TAG redesign-lockstep-engine-only
  CMAKE_ARGS
  -D LAPLACE_ENABLE_TESTING=OFF)
FetchContent_MakeAvailable(laplace)

add_executable(${NAME})
target_sources(${NAME} PRIVATE main.cpp)
target_link_libraries(${NAME} PRIVATE laplace::laplace)

enable_testing()
add_test(NAME ${NAME}-run COMMAND ${NAME})
set_tests_properties(${NAME}-run PROPERTIES TIMEOUT "15")
