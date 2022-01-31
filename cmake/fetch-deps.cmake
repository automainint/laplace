include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY  ${LAPLACE_GOOGLETEST_REPO}
  GIT_TAG         ${LAPLACE_GOOGLETEST_TAG})

set(BUILD_GMOCK OFF)

FetchContent_Declare(
  benchmark
  GIT_REPOSITORY  ${LAPLACE_GOOGLEBENCHMARK_REPO}
  GIT_TAG         ${LAPLACE_GOOGLEBENCHMARK_TAG})

set(BENCHMARK_ENABLE_TESTING OFF)

FetchContent_Declare(
  wolfssl
  GIT_REPOSITORY  ${LAPLACE_WOLFSSL_REPO}
  GIT_TAG         ${LAPLACE_WOLFSSL_TAG})

set(WOLFSSL_RABBIT      yes)
set(WOLFSSL_ECC         yes)
set(WOLFSSL_EXAMPLES    no)
set(WOLFSSL_CRYPT_TESTS no)

FetchContent_Declare(
  freetype
  GIT_REPOSITORY  ${LAPLACE_FREETYPE_REPO}
  GIT_TAG         ${LAPLACE_FREETYPE_TAG})

set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB       ON)
set(CMAKE_DISABLE_FIND_PACKAGE_BZip2      ON)
set(CMAKE_DISABLE_FIND_PACKAGE_PNG        ON)
set(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz   ON)
set(CMAKE_DISABLE_FIND_PACKAGE_BrotliDec  ON)

FetchContent_MakeAvailable(
  googletest benchmark wolfssl freetype)
