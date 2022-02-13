add_library(${LAPLACE_BUILD} INTERFACE)

if(LAPLACE_VERBOSE)
  target_compile_definitions(${LAPLACE_BUILD} INTERFACE LAPLACE_VERBOSE)
endif()

if(LAPLACE_UNSAFE)
  target_compile_definitions(${LAPLACE_BUILD} INTERFACE LAPLACE_UNSAFE)
endif()

target_compile_definitions(${LAPLACE_BUILD} INTERFACE _CONSOLE _UNICODE UNICODE)

find_package(Threads REQUIRED)

if(WIN32)
  target_link_libraries(${LAPLACE_BUILD} INTERFACE Threads::Threads ws2_32)
else()
  target_link_libraries(${LAPLACE_BUILD} INTERFACE Threads::Threads)
endif()

if(MSVC)
  target_compile_options(${LAPLACE_BUILD} INTERFACE /constexpr:steps20000000)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(${LAPLACE_BUILD} INTERFACE -fconstexpr-steps=20000000)
endif()

if(UNIX AND NOT APPLE)
  find_package(X11 REQUIRED)
  target_include_directories(${LAPLACE_BUILD} INTERFACE ${X11_INCLUDE_DIR})
  target_link_libraries(${LAPLACE_BUILD} INTERFACE ${X11_LIBRARIES} -ldl)
endif()

if(LAPLACE_ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${LAPLACE_BUILD}
                           INTERFACE
                           -O0 -g --coverage
                           -fsanitize=undefined,address)

    target_link_options(${LAPLACE_BUILD}
                        INTERFACE
                        --coverage
                        -fsanitize=undefined,address)
  endif()
elseif(LAPLACE_ENABLE_TESTING)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${LAPLACE_BUILD}
                           INTERFACE
                           -O0 -fsanitize=undefined,address)

    target_link_options(${LAPLACE_BUILD}
                        INTERFACE
                        -fsanitize=undefined,address)
  elseif(MSVC)
    target_compile_options(${LAPLACE_BUILD} INTERFACE /Od)
  endif()
else()
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${LAPLACE_BUILD} INTERFACE -O3)
  elseif(MSVC)
    target_compile_options(${LAPLACE_BUILD} INTERFACE /O2)
  endif()
endif()
