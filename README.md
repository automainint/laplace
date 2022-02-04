#   The Laplace engine
[![build-and-test][build-badge]][build-link]
[![codecov][codecov-badge]][codecov-link]

The quality of a real-time strategy game relies on low-latency responsiveness and vertical scalability.
Vertical scalability requires parallelism on the local instance and lockstep networking.
Lockstep systems require a totally deterministic engine. In addition, extensibility is a useful feature.

Laplace is an interactive multimedia framework that provides the deterministic and extensible engine
with concurrency support and lockstep synchronization.

Official repository: [GitHub][laplace-link]

    Work in progress...
    The project is far from complete.

### Dependencies
- C++20 [MSVC][msvc-link]/[Clang][clang-link]/[GCC][gcc-link] for the compilation;
- [Python3][python-link], [Cmake][cmake-link], [Git][git-link] for building and tools;
- [OpenGL][opengl-link] for all the graphics;
- [wolfSSL][wolfssl-link] for encryption algorithms (ECC/Rabbit);
- [FreeType2][freetype-link] for font rendering;
- [googletest][googletest-link] for the test suite;
- [google/benchmark][benchmark-link] for benchmarks.

##  How to build
To build the project CMake 3.16, Git and Python 3 are required.
Make sure your compiler [supports][compiler-support-link] C++20.

### With provided build script
- Execute the build script.

```shell
python3 build.py
```
Done!

### With CMake
- Configure and build the project.

```shell
cmake -D CMAKE_BUILD_TYPE=Release -B build -S .
cmake --build build --config Release
```

Laplace CMake configuration options:
- `LAPLACE_ENABLE_EXE` - build the executable. `ON` by default.
- `LAPLACE_ENABLE_LIB` - build the static library. `OFF` by default.
- `LAPLACE_ENABLE_OBJ` - enable object library. `OFF` by default.
- `LAPLACE_ENABLE_TESTING` - enable testing. `OFF` by default.
- `LAPLACE_ENABLE_COVERAGE` - enable coverage reporting. `OFF` by default.
- `LAPLACE_CODEGEN` - perform code generation.
Will generate embedded data code and OpenGL interface code. `ON` by default.
- `LAPLACE_VERBOSE` - enable verbose logging of Laplace. `ON` by default.
- `LAPLACE_UNSAFE` - disable safety checks of Laplace. `OFF` by default.
- `LAPLACE_FETCH_DEPENDENCIES` - fetch dependencies from source.
Set to `OFF` is you have installed dependencies. `ON` by default.

Tests will be created if `LAPLACE_ENABLE_TESTING` or `LAPLACE_ENABLE_COVERAGE` is `ON`.

##  Hacking
If you want to build the source code by hand, follow the instructions below:
- Use CMake to build and install dependencies.
  - Set `LAPLACE_ENABLE_EXE=OFF` to build dependencies only.
  - Set `CMAKE_INSTALL_PREFIX` to specify install folder.

```shell
cmake -D LAPLACE_ENABLE_EXE=OFF -D CMAKE_INSTALL_PREFIX=thirdparty -S . -B build-deps
cmake --build build-deps
cmake --install build-deps
```

- Use `tools/gen-gl.py` to generate OpenGL interface files.
- Use `tools/embed.py` to generate embedded data source files.

```shell
cd tools
python3 gen-gl.py
python3 embed.py
cd ..
```

Add `source` to the project and build manually.

If you change the source code files structure, use `tools/gen-cmake.py` to generate `CMakeLists.txt` files.

[build-badge]:    https://github.com/automainint/laplace/actions/workflows/build-and-test.yml/badge.svg?branch=stable
[build-link]:     https://github.com/automainint/laplace/actions/workflows/build-and-test.yml
[codecov-badge]:  https://codecov.io/gh/automainint/laplace/branch/stable/graph/badge.svg?token=HJF3BUA46B
[codecov-link]:   https://codecov.io/gh/automainint/laplace/branch/stable

[laplace-link]:          https://github.com/automainint/laplace
[msvc-link]:             https://visualstudio.microsoft.com/vs/features/cplusplus
[clang-link]:            https://clang.llvm.org
[gcc-link]:              https://gcc.gnu.org
[python-link]:           https://www.python.org
[cmake-link]:            https://cmake.org
[git-link]:              https://git-scm.com
[opengl-link]:           https://www.khronos.org/registry/OpenGL/index_gl.php
[wolfssl-link]:          https://github.com/wolfSSL/wolfssl
[freetype-link]:         https://gitlab.freedesktop.org/freetype/freetype
[googletest-link]:       https://github.com/google/googletest
[benchmark-link]:        https://github.com/google/benchmark
[compiler-support-link]: https://en.cppreference.com/w/cpp/compiler_support
