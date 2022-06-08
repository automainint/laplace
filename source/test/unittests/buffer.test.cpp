/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/buffer.h"
#include <catch2/catch.hpp>
#include <limits>

namespace laplace::test {
  using std::numeric_limits;

  TEST_CASE("Create buffer", "[buffer]") {
    REQUIRE(!buffer {}.is_error());
  }

  TEST_CASE("Buffer get chunk size", "[buffer]") {
    REQUIRE(buffer {}.get_chunk_size() == buffer::default_chunk_size);
  }

  TEST_CASE("Buffer set chunk size", "[buffer]") {
    REQUIRE(buffer {}.set_chunk_size(10).get_chunk_size() == 10);
  }

  TEST_CASE("Buffer set chunk size may fail", "[buffer]") {
    REQUIRE(buffer {}.set_chunk_size(-1).is_error());
  }

  TEST_CASE("Buffer allocate", "[buffer]") {
    REQUIRE(buffer {}.allocate(10) != -1);
  }

  TEST_CASE("Buffer allocate may fail", "[buffer]") {
    REQUIRE(buffer {}.allocate(-1) == -1);
    REQUIRE(buffer {}.allocate(0) == -1);
    REQUIRE(buffer {}.allocate(numeric_limits<ptrdiff_t>::max()) ==
            -1);
    REQUIRE(buffer {}.set_chunk_size(-1).allocate(10) == -1);
  }

  TEST_CASE("Buffer deallocate", "[buffer]") {
    auto buf = buffer {};
    REQUIRE(buf.deallocate(buf.allocate(10)));
  }

  TEST_CASE("Buffer deallocate invalid", "[buffer]") {
    REQUIRE(!buffer {}.deallocate(-1));
    REQUIRE(!buffer {}.deallocate(42));
  }

  TEST_CASE("Buffer deallocate may fail", "[buffer]") {
    auto buf = buffer {}.set_chunk_size(-1);
    REQUIRE(!buf.deallocate(0));
  }

  TEST_CASE("Buffer deallocate twice will fail", "[buffer]") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.deallocate(block));
    REQUIRE(!buf.deallocate(block));
  }

  TEST_CASE("Buffer get value", "[buffer]") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.get(block, 0, -1) == 0);
  }

  TEST_CASE("Buffer get value may fail", "[buffer]") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.get(-1, 0, 42) == 42);
    REQUIRE(buf.get(block + 1, 0, 42) == 42);
    REQUIRE(buf.get(block, -1, 42) == 42);
    REQUIRE(buf.get(block, 10, 42) == 42);
    REQUIRE(buffer {}.set_chunk_size(-1).get(0, 0, 42) == 42);
  }

  TEST_CASE("Buffer set value", "[buffer]") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.set(block, 0, 42));
    REQUIRE(buf.get(block, 0, -1) == 0);
  }

  TEST_CASE("Buffer set value may fail", "[buffer]") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(!buf.set(-1, 0, 42));
    REQUIRE(!buf.set(block + 1, 0, 42));
    REQUIRE(!buf.set(block, -1, 42));
    REQUIRE(!buf.set(block, 10, 42));
    REQUIRE(!buffer {}.set_chunk_size(-1).set(0, 0, 42));
  }

  TEST_CASE("Buffer set value and adjust", "[buffer]") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.set(block, 0, 42));
    buf.adjust();
    REQUIRE(buf.get(block, 0, -1) == 42);
  }

  TEST_CASE("Buffer add delta may fail", "[buffer]") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(!buf.add_delta(-1, 0, 42));
    REQUIRE(!buf.add_delta(block + 1, 0, 42));
    REQUIRE(!buf.add_delta(block, -1, 42));
    REQUIRE(!buf.add_delta(block, 10, 42));
    REQUIRE(!buffer {}.set_chunk_size(-1).add_delta(0, 0, 42));
  }

  TEST_CASE("Buffer add delta and adjust", "[buffer]") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.add_delta(block, 0, 42));
    buf.adjust();
    REQUIRE(buf.get(block, 0, -1) == 42);
  }

  TEST_CASE("Buffer add delta twice and adjust", "[buffer]") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.add_delta(block, 0, 20));
    REQUIRE(buf.add_delta(block, 0, 22));
    buf.adjust();
    REQUIRE(buf.get(block, 0, -1) == 42);
  }

  TEST_CASE("Buffer set value and add delta and adjust", "[buffer]") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.set(block, 0, 20));
    REQUIRE(buf.add_delta(block, 0, 22));
    buf.adjust();
    REQUIRE(buf.get(block, 0, -1) == 42);
  }

  TEST_CASE("Buffer adjust one chunk", "[buffer]") {
    auto buf   = buffer {}.set_chunk_size(100);
    auto block = buf.allocate(100);
    for (int i = 0; i < 100; ++i) std::ignore = buf.set(block, i, i);
    REQUIRE(!buf.adjust_chunk());
    for (int i = 0; i < 100; ++i) REQUIRE(buf.get(block, i, -1) == i);
  }

  TEST_CASE("Buffer adjust one chunk may fail", "[buffer]") {
    auto buf = buffer {}.set_chunk_size(-1);
    REQUIRE(!buf.adjust_chunk());
  }

  TEST_CASE("Buffer adjust by chunks", "[buffer]") {
    auto buf   = buffer {}.set_chunk_size(10);
    auto block = buf.allocate(100);
    for (int i = 0; i < 100; ++i) std::ignore = buf.set(block, i, i);
    while (buf.adjust_chunk()) { }
    for (int i = 0; i < 100; ++i) REQUIRE(buf.get(block, i, -1) == i);
  }

  TEST_CASE("Buffer allocate two blocks", "[buffer]") {
    auto buf = buffer {};
    auto foo = buf.allocate(10);
    auto bar = buf.allocate(10);
    REQUIRE(foo != -1);
    REQUIRE(bar != -1);
    REQUIRE(foo != bar);
  }

  TEST_CASE("Buffer fill two blocks", "[buffer]") {
    auto buf = buffer {};
    auto foo = buf.allocate(10);
    auto bar = buf.allocate(10);
    REQUIRE(buf.set(foo, 0, 42));
    REQUIRE(buf.set(bar, 0, 42));
    buf.adjust();
    REQUIRE(buf.get(foo, 0, -1) == 42);
    REQUIRE(buf.get(bar, 0, -1) == 42);
  }

  TEST_CASE("Buffer get size", "[buffer]") {
    REQUIRE(buffer {}.get_size() == 0);
  }

  TEST_CASE("Buffer size with 2 blocks", "[buffer]") {
    auto buf    = buffer {};
    std::ignore = buf.allocate(20);
    std::ignore = buf.allocate(22);
    REQUIRE(buf.get_size() == 42);
  }

  TEST_CASE("Buffer size after deallocate one", "[buffer]") {
    auto buf = buffer {};
    REQUIRE(buf.deallocate(buf.allocate(10)));
    REQUIRE(buf.get_size() == 10);
  }

  TEST_CASE("Buffer size after deallocate and reallocate one",
            "[buffer]") {
    auto buf = buffer {};
    REQUIRE(buf.deallocate(buf.allocate(10)));
    std::ignore = buf.allocate(10);
    REQUIRE(buf.get_size() == 10);
  }

  TEST_CASE("Buffer deallocate efficiency", "[buffer]") {
    auto buf    = buffer {};
    std::ignore = buf.allocate(10);
    REQUIRE(buf.deallocate(buf.allocate(10)));
    std::ignore = buf.allocate(10);
    REQUIRE(buf.get_size() == 20);
  }
}
