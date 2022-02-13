option(LAPLACE_ENABLE_EXE "Enable building executable" ON)
option(LAPLACE_ENABLE_LIB "Enable building static library" OFF)
option(LAPLACE_ENABLE_OBJ "Enable object library" OFF)
option(LAPLACE_ENABLE_TESTING "Enable testing" OFF)
option(LAPLACE_ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(LAPLACE_CODEGEN "Perform code generation" ON)
option(LAPLACE_VERBOSE "Enable verbose logging" ON)
option(LAPLACE_UNSAFE "Disable safety checks" OFF)
option(LAPLACE_FETCH_DEPENDENCIES "Fetch dependencies from source" ON)

option(LAPLACE_ENABLE_CORE "Enable laplace/core" ON)
option(LAPLACE_ENABLE_COROUTINE "Enable laplace/coroutine" ON)
option(LAPLACE_ENABLE_MATH "Enable laplace/math" ON)
option(LAPLACE_ENABLE_ENGINE "Enable laplace/engine" ON)
option(LAPLACE_ENABLE_NETWORK "Enable laplace/network" ON)
option(LAPLACE_ENABLE_FORMAT "Enable laplace/format" ON)
option(LAPLACE_ENABLE_UI "Enable laplace/ui" ON)
option(LAPLACE_ENABLE_PLATFORM "Enable laplace/platform" ON)
option(LAPLACE_ENABLE_GRAPHICS "Enable laplace/graphics" ON)
option(LAPLACE_ENABLE_RENDER "Enable laplace/render" ON)
option(LAPLACE_ENABLE_STEM "Enable laplace/stem" ON)

option(LAPLACE_ENABLE_OPENGL "Enable generated OpenGL interfaces" ON)
option(LAPLACE_ENABLE_EMBEDDED "Enable generated embedded data" ON)

set(LAPLACE_GOOGLETEST_TAG "release-1.11.0" CACHE STRING "googletest git tag")
set(LAPLACE_GOOGLEBENCHMARK_TAG "v1.5.3" CACHE STRING "google/benchmark git tag")
set(LAPLACE_WOLFSSL_TAG "v4.8.1-stable" CACHE STRING "wolfSSL git tag")
set(LAPLACE_FREETYPE_TAG "VER-2-10-4" CACHE STRING "FreeType git tag")

set(LAPLACE_GOOGLETEST_REPO
    "https://github.com/google/googletest.git"
    CACHE STRING "googletest git repository")

set(LAPLACE_GOOGLEBENCHMARK_REPO
    "https://github.com/google/benchmark.git"
    CACHE STRING "google/benchmark git repository")

set(LAPLACE_WOLFSSL_REPO
    "https://github.com/wolfSSL/wolfssl.git"
    CACHE STRING "wolfSSL git repository")

set(LAPLACE_FREETYPE_REPO
    "https://gitlab.freedesktop.org/freetype/freetype.git"
    CACHE STRING "FreeType git repository")

set(LAPLACE_PROJECT laplace)
set(LAPLACE_OBJ laplace-obj)
set(LAPLACE_LIB laplace)
set(LAPLACE_BUILD laplace-build)
set(LAPLACE_SOCKETS laplace-sockets)
set(LAPLACE_UNITTESTS laplace-unittests)
set(LAPLACE_DEPENDENCIES laplace-dependencies)
set(QUADWAR_OBJ quadwar-obj)
set(QUADWAR_EXE quadwar)

set(LAPLACE_VENDOR "automainint")

set(LAPLACE_TARGETS laplace-targets)
set(LAPLACE_COMPONENT_DEVELOPMENT laplace-development)
set(QUADWAR_COMPONENT_RUNTIME quadwar-runtime)
