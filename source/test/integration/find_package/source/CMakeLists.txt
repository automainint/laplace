cmake_minimum_required(VERSION 3.16)
set(NAME laplace-integration-find-package)
project(${NAME})

find_package(laplace REQUIRED)

add_executable(${NAME})
target_sources(${NAME} PRIVATE main.cpp)
target_link_libraries(${NAME} PRIVATE laplace::laplace)
install(TARGETS ${NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
