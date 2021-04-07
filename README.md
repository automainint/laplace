#   The Laplace engine
The quality of the real-time strategy game depends on low-latency responsiveness and vertical scalability. Vertical scalability requires parallelism on the local instance and lockstep networking. Lockstep systems require a totally deterministic engine. In addition, extensibility is a useful feature.

Laplace is an interactive multimedia framework that provides the deterministic and extensible engine with concurrency support and lockstep synchronization.

Official repository: [GitHub][laplace-link]

    Work in progress...
    The project is far from complete.

### Dependencies
- C++20 [MSVC][msvc-link]/[Clang][clang-link]/[GCC][gcc-link] for the compilation;
- [Python3][python-link], [Cmake][cmake-link], [Git][git-link] for the building and tools;
- [OpenGL][opengl-link] for all the graphics;
- [wolfSSL][wolfssl-link] for encryption algorithms (ECC/Rabbit);
- [FreeType2][freetype-link] for the font rendering;
- [bzip2][bzip2-link] for the archivation;
- [googletest][googletest-link] for the test suite;
- [google/benchmark][benchmark-link] for the benchmarks;
- [Blender][blender-link] for the content creating.

##  How to build
To build the project CMake and Python 3 are required.

### With provided build script
Execute the build script.

```shell
python3 build.py
```
Done!

### With Cmake
- Use `tools/gen-gl.py` to generate the OpenGL interface files.
- Use `tools/embed.py` to generate the embedded data source files.
- Build the project.

```shell
cd tools
python3 gen-gl.py
python3 embed.py
cd ..

cmake -D CMAKE_BUILD_TYPE=Release -B build -S .
cmake --build build --config Release
```

### By hand
- Use `tools/build-deps.py` to build the dependencies.
- Use `tools/gen-gl.py` to generate the OpenGL interface files.
- Use `tools/embed.py` to generate the embedded data source files.

```shell
cd tools
python3 build-deps.py
python3 gen-gl.py
python3 embed.py
cd ..
```

Add `source` to the project and build manually. Required headers will be in `include` folder, libraries will be in `lib` folder.

[laplace-link]:     https://github.com/automainint/laplace
[msvc-link]:        https://visualstudio.microsoft.com/vs/features/cplusplus
[clang-link]:       https://clang.llvm.org
[gcc-link]:         https://gcc.gnu.org
[python-link]:      https://www.python.org
[cmake-link]:       https://cmake.org
[git-link]:         https://git-scm.com
[opengl-link]:      https://www.khronos.org/registry/OpenGL/index_gl.php
[wolfssl-link]:     https://github.com/wolfSSL/wolfssl
[freetype-link]:    https://gitlab.freedesktop.org/freetype/freetype
[bzip2-link]:       https://gitlab.com/federicomenaquintero/bzip2
[googletest-link]:  https://github.com/google/googletest
[benchmark-link]:   https://github.com/google/benchmark
[blender-link]:     https://www.blender.org
