find_package(Threads REQUIRED)

add_library(${LAPLACE_CONFIG} INTERFACE)

if(WIN32)
  target_compile_definitions(${LAPLACE_CONFIG} INTERFACE _CONSOLE _UNICODE UNICODE)
  target_link_libraries(${LAPLACE_CONFIG} INTERFACE Threads::Threads ws2_32)
else()
  target_link_libraries(${LAPLACE_CONFIG} INTERFACE Threads::Threads)
endif()

if(LAPLACE_ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      ${LAPLACE_CONFIG}
        INTERFACE
          -O0 -g --coverage
          -fsanitize=undefined,address
    )

    target_link_options(
      ${LAPLACE_CONFIG}
        INTERFACE
           --coverage
           -fsanitize=undefined,address
    )
  endif()
elseif(LAPLACE_ENABLE_TESTING)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
      ${LAPLACE_CONFIG}
        INTERFACE
          -O0 -fsanitize=undefined,address
    )

    target_link_options(
      ${LAPLACE_CONFIG}
        INTERFACE
           -fsanitize=undefined,address
    )
  elseif(MSVC)
    target_compile_options(${LAPLACE_CONFIG} INTERFACE /Od)
  endif()
else()
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${LAPLACE_CONFIG} INTERFACE -O3)
  elseif(MSVC)
    target_compile_options(${LAPLACE_CONFIG} INTERFACE /O2)
  endif()
endif()
