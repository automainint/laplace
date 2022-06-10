/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "../../laplace/buffer.h"
#include <catch2/catch.hpp>
#include <limits>
#include <thread>

namespace laplace::test {
  using std::numeric_limits, std::thread, std::vector;

  TEST_CASE("Create buffer") {
    REQUIRE(!buffer {}.is_error());
  }

  TEST_CASE("Buffer get chunk size") {
    REQUIRE(buffer {}.get_chunk_size() == buffer::default_chunk_size);
  }

  TEST_CASE("Buffer set chunk size") {
    REQUIRE(buffer {}.set_chunk_size(10).get_chunk_size() == 10);
  }

  TEST_CASE("Buffer set chunk size may fail") {
    REQUIRE(buffer {}.set_chunk_size(-1).is_error());
  }

  TEST_CASE("Buffer propagate error") {
    REQUIRE(
        buffer {}.set_chunk_size(-1).set_chunk_size(10).is_error());
  }

  TEST_CASE("Buffer allocate") {
    REQUIRE(buffer {}.allocate(10) != id_undefined);
  }

  TEST_CASE("Buffer allocate may fail") {
    REQUIRE(buffer {}.allocate(-1) == id_undefined);
    REQUIRE(buffer {}.allocate(0) == id_undefined);
    REQUIRE(buffer {}.allocate(numeric_limits<ptrdiff_t>::max()) ==
            -1);
    REQUIRE(buffer {}.set_chunk_size(-1).allocate(10) ==
            id_undefined);
  }

  TEST_CASE("Buffer deallocate") {
    auto buf = buffer {};
    REQUIRE(buf.deallocate(buf.allocate(10)));
  }

  TEST_CASE("Buffer deallocate invalid") {
    REQUIRE(!buffer {}.deallocate(-1));
    REQUIRE(!buffer {}.deallocate(42));
  }

  TEST_CASE("Buffer deallocate may fail") {
    auto buf = buffer {}.set_chunk_size(-1);
    REQUIRE(!buf.deallocate(0));
  }

  TEST_CASE("Buffer deallocate twice will fail") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.deallocate(block));
    REQUIRE(!buf.deallocate(block));
  }

  TEST_CASE("Buffer get value") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.get(block, 0, -1) == 0);
  }

  TEST_CASE("Buffer get value may fail") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.get(-1, 0, 42) == 42);
    REQUIRE(buf.get(block + 1, 0, 42) == 42);
    REQUIRE(buf.get(block, -1, 42) == 42);
    REQUIRE(buf.get(block, 10, 42) == 42);
    REQUIRE(buffer {}.set_chunk_size(-1).get(0, 0, 42) == 42);
  }

  TEST_CASE("Buffer set value") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.set(block, 0, 42));
    REQUIRE(buf.get(block, 0, -1) == 0);
  }

  TEST_CASE("Buffer set value may fail") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(!buf.set(-1, 0, 42));
    REQUIRE(!buf.set(block + 1, 0, 42));
    REQUIRE(!buf.set(block, -1, 42));
    REQUIRE(!buf.set(block, 10, 42));
    REQUIRE(!buffer {}.set_chunk_size(-1).set(0, 0, 42));
  }

  TEST_CASE("Buffer set value and adjust") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.set(block, 0, 42));
    while (buf.adjust()) { }
    REQUIRE(buf.get(block, 0, -1) == 42);
  }

  TEST_CASE("Buffer add delta may fail") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(!buf.add(-1, 0, 42));
    REQUIRE(!buf.add(block + 1, 0, 42));
    REQUIRE(!buf.add(block, -1, 42));
    REQUIRE(!buf.add(block, 10, 42));
    REQUIRE(!buffer {}.set_chunk_size(-1).add(0, 0, 42));
  }

  TEST_CASE("Buffer add delta and adjust") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.add(block, 0, 42));
    while (buf.adjust()) { }
    REQUIRE(buf.get(block, 0, -1) == 42);
  }

  TEST_CASE("Buffer add delta twice and adjust") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.add(block, 0, 20));
    REQUIRE(buf.add(block, 0, 22));
    while (buf.adjust()) { }
    REQUIRE(buf.get(block, 0, -1) == 42);
  }

  TEST_CASE("Buffer set value and add delta and adjust") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.set(block, 0, 20));
    REQUIRE(buf.add(block, 0, 22));
    while (buf.adjust()) { }
    REQUIRE(buf.get(block, 0, -1) == 42);
  }

  TEST_CASE("Buffer adjust one chunk") {
    auto buf   = buffer {}.set_chunk_size(100);
    auto block = buf.allocate(100);
    for (int i = 0; i < 100; ++i) std::ignore = buf.set(block, i, i);
    REQUIRE(!buf.adjust());
    for (int i = 0; i < 100; ++i) {
      REQUIRE(buf.get(block, i, -1) == i);
    }
  }

  TEST_CASE("Buffer adjust one chunk may fail") {
    auto buf = buffer {}.set_chunk_size(-1);
    REQUIRE(!buf.adjust());
  }

  TEST_CASE("Buffer adjust by chunks") {
    auto buf   = buffer {}.set_chunk_size(10);
    auto block = buf.allocate(100);
    for (int i = 0; i < 100; ++i) std::ignore = buf.set(block, i, i);
    while (buf.adjust()) { }
    for (int i = 0; i < 100; ++i) REQUIRE(buf.get(block, i, -1) == i);
  }

  TEST_CASE("Buffer adjust by chunks twice") {
    auto buf   = buffer {}.set_chunk_size(10);
    auto block = buf.allocate(100);
    for (int i = 0; i < 100; ++i) std::ignore = buf.set(block, i, i);
    while (buf.adjust()) { }
    buf.adjust_done();
    for (int i = 0; i < 100; ++i) std::ignore = buf.add(block, i, i);
    while (buf.adjust()) { }
    for (int i = 0; i < 100; ++i) {
      REQUIRE(buf.get(block, i, -1) == i * 2);
    }
  }

  TEST_CASE("Buffer allocate two blocks") {
    auto buf = buffer {};
    auto foo = buf.allocate(10);
    auto bar = buf.allocate(10);
    REQUIRE(foo != id_undefined);
    REQUIRE(bar != id_undefined);
    REQUIRE(foo != bar);
  }

  TEST_CASE("Buffer fill two blocks") {
    auto buf = buffer {};
    auto foo = buf.allocate(10);
    auto bar = buf.allocate(10);
    REQUIRE(buf.set(foo, 0, 42));
    REQUIRE(buf.set(bar, 0, 42));
    while (buf.adjust()) { }
    REQUIRE(buf.get(foo, 0, -1) == 42);
    REQUIRE(buf.get(bar, 0, -1) == 42);
  }

  TEST_CASE("Buffer get size") {
    REQUIRE(buffer {}.get_size() == 0);
  }

  TEST_CASE("Buffer size with 2 blocks") {
    auto buf    = buffer {};
    std::ignore = buf.allocate(20);
    std::ignore = buf.allocate(22);
    REQUIRE(buf.get_size() == 42);
  }

  TEST_CASE("Buffer size after deallocate one") {
    auto buf = buffer {};
    REQUIRE(buf.deallocate(buf.allocate(10)));
    REQUIRE(buf.get_size() == 10);
  }

  TEST_CASE("Buffer size after deallocate and reallocate one") {
    auto buf = buffer {};
    REQUIRE(buf.deallocate(buf.allocate(10)));
    std::ignore = buf.allocate(10);
    REQUIRE(buf.get_size() == 10);
  }

  TEST_CASE("Buffer deallocate efficiency") {
    auto buf    = buffer {};
    std::ignore = buf.allocate(10);
    REQUIRE(buf.deallocate(buf.allocate(10)));
    std::ignore = buf.allocate(10);
    REQUIRE(buf.get_size() == 20);
  }

  TEST_CASE("Buffer add delta concurrency") {
    auto buf = buffer {};
    auto id  = buf.allocate(1);

    auto pool = vector<thread> {};
    for (int i = 0; i < 100; i++)
      pool.emplace_back(thread { [&]() {
        for (int j = 0; j < 100; j++) {
          std::ignore = buf.add(id, 0, 1);
          std::ignore = buf.add(id, 0, -1);
        }
        std::ignore = buf.add(id, 0, 1);
      } });
    for (auto &t : pool) t.join();

    while (buf.adjust()) { }

    REQUIRE(buf.get(id, 0, -1) == 100);
  }

  TEST_CASE("Buffer add delta concurrency harder") {
    auto buf = buffer {};
    auto id  = buf.allocate(1);

    auto pool = vector<thread> {};
    for (int i = 0; i < 1000; i++)
      pool.emplace_back(thread { [&]() {
        for (int j = 0; j < 100; j++) {
          std::ignore = buf.add(id, 0, 1);
          std::ignore = buf.add(id, 0, -1);
        }
        std::ignore = buf.add(id, 0, 1);
      } });
    for (auto &t : pool) t.join();

    while (buf.adjust()) { }

    REQUIRE(buf.get(id, 0, -1) == 1000);
  }

  TEST_CASE("Buffer adjust concurrency") {
    auto buf = buffer {}.set_chunk_size(10);
    auto id  = buf.allocate(10000);
    for (int i = 0; i < 10000; i++) std::ignore = buf.set(id, i, i);

    auto pool = vector<thread> {};
    for (int i = 0; i < 1000; i++)
      pool.emplace_back(
          thread { [&]() { std::ignore = buf.adjust(); } });
    for (auto &t : pool) t.join();

    for (int i = 0; i < 10000; i++) {
      REQUIRE(buf.get(id, i, -1) == i);
    }
  }

  TEST_CASE("Buffer adjust concurrency harder") {
    auto buf = buffer {}.set_chunk_size(1);
    auto id  = buf.allocate(10000);
    for (int i = 0; i < 10000; i++) std::ignore = buf.set(id, i, i);

    auto pool = vector<thread> {};
    for (int i = 0; i < 10000; i++)
      pool.emplace_back(
          thread { [&]() { std::ignore = buf.adjust(); } });
    for (auto &t : pool) t.join();

    for (int i = 0; i < 10000; i++) {
      REQUIRE(buf.get(id, i, -1) == i);
    }
  }
}
