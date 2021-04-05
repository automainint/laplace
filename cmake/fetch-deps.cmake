include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
)

FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
)

set(BENCHMARK_ENABLE_TESTING OFF)

FetchContent_Declare(
  wolfssl
  GIT_REPOSITORY https://github.com/wolfSSL/wolfssl.git
)

set(WOLFSSL_RABBIT yes)
set(WOLFSSL_ECC yes)

FetchContent_Declare(
  bzip2
  GIT_REPOSITORY https://gitlab.com/federicomenaquintero/bzip2.git
)

set(ENABLE_SHARED_LIB OFF)
set(ENABLE_STATIC_LIB ON)

FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
)

set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB ON)
set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 ON)
set(CMAKE_DISABLE_FIND_PACKAGE_PNG ON)
set(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz ON)
set(CMAKE_DISABLE_FIND_PACKAGE_BrotliDec ON)

FetchContent_MakeAvailable(
  googletest benchmark wolfssl bzip2 freetype
)
