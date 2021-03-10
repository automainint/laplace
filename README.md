#   The Laplace engine
The quality of the real-time strategy game depends on low-latency responsiveness and scalability. Scalability requires parallelism on the local instance and lockstep networking. Lockstep systems require a totally deterministic engine. In addition, extensibility is a useful feature.

Laplace is an interactive multimedia framework that provides the deterministic and extensible engine with concurrency support and lockstep synchronization.

    Work in progress...
    The project is far from complete.

**Dependencies:** C++20 [MSVC](https://visualstudio.microsoft.com/vs/features/cplusplus/)/[Clang](https://clang.llvm.org/)/[GCC](https://gcc.gnu.org/) for the compilation; [Python3](https://www.python.org/), [Cmake](https://cmake.org/), [Git](https://git-scm.com/) for the building and tools; [OpenGL](https://www.khronos.org/registry/OpenGL/index_gl.php) for all the graphics; [wolfSSL](https://github.com/wolfSSL/wolfssl) for encryption algorithms (ECC/Rabbit); [FreeType2](https://gitlab.freedesktop.org/freetype/freetype) for the font rendering; [googletest](https://github.com/google/googletest) for the test suite; [google/benchmark](https://github.com/google/benchmark) for the benchmarks; [Blender](https://www.blender.org/) for the content creating.

##  How to build
To build the project CMake and Python 3 are required.

### With CMake
Execute `build.py`.

    python3 build.py

Done!

### By hand
Use `tools/build-deps.py` to build the dependencies.

    cd tools
    python3 build-deps.py

Use `tools/gen-gl.py` to generate the OpenGL interface files.

    python3 gen-gl.py

Use `tools/embed.py` to generate the embedded data source files.

    python3 embed.py
    cd ..

Add `source` to the project and build manually. Required headers will be in `include` folder, libraries will be in `lib` folder.
