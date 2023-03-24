#   The Laplace engine
The quality of a real-time strategy game relies on low-latency responsiveness and vertical scalability.
Vertical scalability requires parallelism on the local instance and lockstep networking.
Lockstep systems require a totally deterministic engine. In addition, extensibility is a useful feature.

Laplace is an interactive multimedia framework that provides a deterministic and extensible engine
with concurrency support and lockstep synchronization.

    Work in progress...
    The project is far from complete.

##  Related repositories
- [kit](https://github.com/automainint/kit) - a collection of C libraries.
- [eval](https://github.com/automainint/eval) - deterministic math and algorithms.
- [peer](https://github.com/automainint/peer) - network synchronization library.
- [quadwar](https://github.com/automainint/quadwar) - playground for new features and experiments.

##  Hacking
### Build the source code
```shell
cmake -B build -S .
cmake --build build
```

Laplace CMake configuration options:
- `LAPLACE_ENABLE_CUSTOM_ALLOC_DISPATCH` - enable custom allocator dispatch function. `OFF` by default.
- `LAPLACE_ENABLE_STATIC_DISPATCH` - enable static action dispatch. `ON` by default.
- `LAPLACE_ENABLE_TESTING` - enable testing. `ON` by default.

### Run tests
```shell
./build/laplace_test_suite
```
