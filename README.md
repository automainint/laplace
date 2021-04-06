#   The Laplace engine
The quality of the real-time strategy game depends on low-latency responsiveness and vertical scalability. Vertical scalability requires parallelism on the local instance and lockstep networking. Lockstep systems require a totally deterministic engine. In addition, extensibility is a useful feature.

Laplace is an interactive multimedia framework that provides the deterministic and extensible engine with concurrency support and lockstep synchronization.

Official repository: [GitHub](https://github.com/automainint/laplace)

    Work in progress...
    The project is far from complete.

**Dependencies:** C++20 [MSVC](https://visualstudio.microsoft.com/vs/features/cplusplus/)/[Clang](https://clang.llvm.org/)/[GCC](https://gcc.gnu.org/) for the compilation; [Python3](https://www.python.org/), [Cmake](https://cmake.org/), [Git](https://git-scm.com/) for the building and tools; [OpenGL](https://www.khronos.org/registry/OpenGL/index_gl.php) for all the graphics; [wolfSSL](https://github.com/wolfSSL/wolfssl) for encryption algorithms (ECC/Rabbit); [FreeType2](https://gitlab.freedesktop.org/freetype/freetype) for the font rendering; [googletest](https://github.com/google/googletest) for the test suite; [google/benchmark](https://github.com/google/benchmark) for the benchmarks; [Blender](https://www.blender.org/) for the content creating.

##  How to build
To build the project CMake and Python 3 are required.

### With provided build script
- Execute the build script.
  - `python3 build.py`
- Done!

### With Cmake
- Generate the OpenGL interface files.
  - `cd tools`
  - `python3 gen-gl.py`
- Generate the embedded data source files.
  - `python3 embed.py`
  - `cd ..`
- Build the project.
  - `cmake -D CMAKE_BUILD_TYPE=Release -B build -S .`
  - `cmake --build build --config Release`

### By hand
- Build the dependencies.
  - `cd tools`
  - `python3 build-deps.py`
- Generate the OpenGL interface files.
  - `python3 gen-gl.py`
- Generate the embedded data source files.
  - `python3 embed.py`
  - `cd ..`
- Add `source` to the project and build manually. Required headers will be in `include` folder, libraries will be in `lib` folder.
