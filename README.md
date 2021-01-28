#   The Laplace project
Multimedia framework focused on RTS games developlent. Provide the deterministic engine with concurrency support and lockstep synchronization networking.

    Work in progress...

**Traits:** complete determinism, lockstep synchronization, concurrency support, scalability, extendability.

**Dependencies:** C++20 [MSVC](https://visualstudio.microsoft.com/ru/vs/features/cplusplus/)/[Clang](https://clang.llvm.org/)/[GCC](https://gcc.gnu.org/) for the compilation; [Python3](https://www.python.org/), [Cmake](https://cmake.org/), [Git](https://git-scm.com/) for the building and tools; [OpenGL](https://www.khronos.org/registry/OpenGL/index_gl.php) for all the graphics; [wolfSSL](https://github.com/wolfSSL/wolfssl) for encryption algorithms (ECC/Rabbit); [FreeType2](https://gitlab.freedesktop.org/freetype/freetype) for the font rendering; [googletest](https://github.com/google/googletest) for the test suite; [google/benchmark](https://github.com/google/benchmark) for the benchmarks; [Blender](https://www.blender.org/) for the content creating.

##  How to build
Use `thirdparty/update.sh` to download and `thirdparty/rebuild.sh` to build the repos. Use `thirdparty/gengl.py` to automatically generate the OpenGL interface files. Add `source/` to the project and build manually.
