#   The Laplace engine
[![linux-build][linux-build-badge]][linux-build-link]

The quality of a real-time strategy game relies on low-latency responsiveness and vertical scalability. Vertical scalability requires parallelism on the local instance and lockstep networking. Lockstep systems require a totally deterministic engine. In addition, extensibility is a useful feature.

Laplace is an interactive multimedia framework that provides the deterministic and extensible engine with concurrency support and lockstep synchronization.

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
To build the project CMake 3.18, Git and Python 3 are required.

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
- `LAPLACE_ENABLE_TESTING` - enable testing. `OFF` by default.
- `LAPLACE_ENABLE_COVERAGE` - enable coverage reporting. `OFF` by default.
- `LAPLACE_VERBOSE` - enable verbose logging of Laplace. `ON` by default.

Tests will be created if `LAPLACE_ENABLE_TESTING` or `LAPLACE_ENABLE_COVERAGE` is `ON`.

To build only dependencies set `LAPLACE_ENABLE_EXE` to `OFF`.

##  Hacking
If you want to build the source code by hand, follow the instructions below:
- Use `tools/build-deps.py` to build dependencies.
- Use `tools/gen-gl.py` to generate OpenGL interface files.
- Use `tools/embed.py` to generate embedded data source files.

```shell
cd tools
python3 build-deps.py
python3 gen-gl.py
python3 embed.py
cd ..
```

Add `source` to the project and build manually. Required headers will be in `include` folder, libraries will be in `lib` folder.

If you change the source code files structure, use `tools/gen-cmake.py` to generate `CMakeLists.txt` files.

##  Contribution
To contribute, follow these steps:
1.  Find an applicable issue or create a new one with feature proposal or bug spotting.
2.  Fork the repository.
3.  Write tests for the issue.
4.  Implement your solution for the issue.
    - Follow [C++ Core Guidelines][guidelines-link].
    - Use [ClangFormat][clang-format-link].
5.  Make sure all tests pass and cover your code.
6.  Make a pull-request to the `dev` branch or to the corresponding feature branch if there is any.

We have just started organizing the project. If you want to help, [contact me][concact-link].

[linux-build-badge]:    https://github.com/automainint/laplace/workflows/linux-build/badge.svg
[linux-build-link]:     https://github.com/automainint/laplace/actions?query=workflow%3Alinux-build

[laplace-link]:       https://github.com/automainint/laplace
[msvc-link]:          https://visualstudio.microsoft.com/vs/features/cplusplus
[clang-link]:         https://clang.llvm.org
[gcc-link]:           https://gcc.gnu.org
[python-link]:        https://www.python.org
[cmake-link]:         https://cmake.org
[git-link]:           https://git-scm.com
[opengl-link]:        https://www.khronos.org/registry/OpenGL/index_gl.php
[wolfssl-link]:       https://github.com/wolfSSL/wolfssl
[freetype-link]:      https://gitlab.freedesktop.org/freetype/freetype
[googletest-link]:    https://github.com/google/googletest
[benchmark-link]:     https://github.com/google/benchmark
[clang-format-link]:  https://clang.llvm.org/docs/ClangFormat.html
[guidelines-link]:    https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines

[concact-link]: https://guattari.ru/contact
