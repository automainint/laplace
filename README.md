#   The Laplace engine
[![build-and-test][build-badge]][build-link]
[![codecov][codecov-badge]][codecov-link]

The quality of a real-time strategy game relies on low-latency responsiveness and vertical scalability.
Vertical scalability requires parallelism on the local instance and lockstep networking.
Lockstep systems require a totally deterministic engine. In addition, extensibility is a useful feature.

Laplace is an interactive multimedia framework that provides a deterministic and extensible engine
with concurrency support and lockstep synchronization.

Official repository: [GitHub][laplace-link]

    Work in progress...
    The project is far from complete.

##  How to build
To build the project CMake 3.16 and Git are required.

### With CMake
- Configure and build the project.

```shell
cmake -D CMAKE_BUILD_TYPE=Release -B build -S .
cmake --build build --config Release
```

Laplace CMake configuration options:
- `LAPLACE_ENABLE_TESTING` - enable testing. `ON` by default.

[build-badge]:    https://github.com/automainint/laplace/actions/workflows/build-and-test.yml/badge.svg?branch=dev
[build-link]:     https://github.com/automainint/laplace/actions/workflows/build-and-test.yml
[codecov-badge]:  https://codecov.io/gh/automainint/laplace/branch/dev/graph/badge.svg?token=HJF3BUA46B
[codecov-link]:   https://codecov.io/gh/automainint/laplace/branch/dev

[laplace-link]:          https://github.com/automainint/laplace
[cmake-link]:            https://cmake.org
[git-link]:              https://git-scm.com
[compiler-support-link]: https://en.cppreference.com/w/cpp/compiler_support
