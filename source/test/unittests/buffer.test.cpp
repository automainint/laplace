#include "../../laplace/buffer.h"
#include <limits>
#include <thread>

#define KIT_TEST_FILE buffer
#include <kit_test/test.h>

namespace laplace::test {
  using std::numeric_limits, std::thread, std::vector;

  TEST("create int buffer") {
    REQUIRE(!buffer {}.error());
  }

  TEST("create byte buffer") {
    REQUIRE(!byte_buffer {}.error());
  }

  TEST("buffer get chunk size") {
    REQUIRE(buffer {}.chunk_size() == buffer::default_chunk_size);
  }

  TEST("buffer set chunk size") {
    REQUIRE(buffer {}.set_chunk_size(10).chunk_size() == 10);
  }

  TEST("buffer set chunk size may fail") {
    REQUIRE(buffer {}.set_chunk_size(-1).error());
  }

  TEST("buffer propagate error") {
    REQUIRE(buffer {}.set_chunk_size(-1).set_chunk_size(10).error());
  }

  TEST("buffer allocate") {
    REQUIRE(buffer {}.allocate(10) != id_undefined);
  }

  TEST("buffer allocate may fail") {
    REQUIRE(buffer {}.allocate(-1) == id_undefined);
    REQUIRE(buffer {}.allocate(0) == id_undefined);
    REQUIRE(buffer {}.allocate(numeric_limits<ptrdiff_t>::max()) ==
            id_undefined);
    REQUIRE(buffer {}.set_chunk_size(-1).allocate(10) ==
            id_undefined);
  }

  TEST("buffer allocate into") {
    REQUIRE(buffer {}.allocate_into(42, 10));
  }

  TEST("buffer allocate into may fail") {
    REQUIRE(!buffer {}.allocate_into(id_undefined, 10));
    REQUIRE(!buffer {}.allocate_into(42, -1));
    REQUIRE(!buffer {}.allocate_into(42, 0));
    REQUIRE(!buffer {}.allocate_into(
        42, numeric_limits<ptrdiff_t>::max()));
    REQUIRE(!buffer {}.set_chunk_size(-1).allocate_into(42, 10));
  }

  TEST("buffer allocate and allocate into") {
    auto buf = buffer {};
    auto id  = buf.allocate(10);
    REQUIRE(buf.allocate_into(id, 100));
  }

  TEST("buffer reserve and allocate") {
    auto      buf      = buffer {};
    ptrdiff_t reserved = 42;
    REQUIRE(buf.reserve(reserved));
    REQUIRE(buf.allocate(10) == reserved);
  }

  TEST("buffer reserve may fail") {
    REQUIRE(!buffer {}.reserve(-1));
  }

  TEST("buffer reserve, allocate into and allocate") {
    auto      buf      = buffer {};
    ptrdiff_t reserved = 42;
    ptrdiff_t id       = 1;
    REQUIRE(buf.reserve(reserved));
    REQUIRE(buf.allocate_into(id, 10));
    REQUIRE(buf.allocate(10) == reserved);
  }

  TEST("buffer reserve, deallocate and allocate") {
    auto      buf      = buffer {};
    ptrdiff_t reserved = 42;
    ptrdiff_t id       = 1;
    REQUIRE(buf.reserve(reserved));
    REQUIRE(buf.allocate_into(id, 10));
    REQUIRE(buf.deallocate(id));
    REQUIRE(buf.allocate(10) == reserved);
  }

  TEST("buffer deallocate") {
    auto buf = buffer {};
    REQUIRE(buf.deallocate(buf.allocate(10)));
  }

  TEST("buffer deallocate invalid") {
    REQUIRE(!buffer {}.deallocate(-1));
    REQUIRE(!buffer {}.deallocate(42));
  }

  TEST("buffer deallocate may fail") {
    auto buf = buffer {}.set_chunk_size(-1);
    REQUIRE(!buf.deallocate(0));
  }

  TEST("buffer deallocate twice will fail") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.deallocate(block));
    REQUIRE(!buf.deallocate(block));
  }

  TEST("buffer get value") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.get(block, 0, -1) == 0);
  }

  TEST("buffer get value may fail") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.get(-1, 0, 42) == 42);
    REQUIRE(buf.get(block + 1, 0, 42) == 42);
    REQUIRE(buf.get(block, -1, 42) == 42);
    REQUIRE(buf.get(block, 10, 42) == 42);
    REQUIRE(buffer {}.set_chunk_size(-1).get(0, 0, 42) == 42);
  }

  TEST("buffer set value") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.set(block, 0, 42));
    REQUIRE(buf.get(block, 0, -1) == 0);
  }

  TEST("buffer set value may fail") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(!buf.set(-1, 0, 42));
    REQUIRE(!buf.set(block + 1, 0, 42));
    REQUIRE(!buf.set(block, -1, 42));
    REQUIRE(!buf.set(block, 10, 42));
    REQUIRE(!buffer {}.set_chunk_size(-1).set(0, 0, 42));
  }

  TEST("buffer set value and adjust") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.set(block, 0, 42));
    while (buf.adjust()) { }
    REQUIRE(buf.get(block, 0, -1) == 42);
  }

  TEST("buffer set value, deallocate and get value") {
    auto buf = buffer {};
    auto foo = buf.allocate(1);
    auto bar = buf.allocate(1);
    REQUIRE(buf.set(bar, 0, 42));
    REQUIRE(buf.deallocate(foo));
    while (buf.adjust()) { }
    REQUIRE(buf.get(bar, 0, -1) == 42);
  }

  TEST("buffer set value, deallocate, allocate and get value") {
    auto buf = buffer {};
    auto id  = buf.allocate(1);
    REQUIRE(buf.set(id, 0, 42));
    while (buf.adjust()) { }
    REQUIRE(buf.deallocate(id));
    id = buf.allocate(1);
    REQUIRE(buf.get(id, 0, -1) == 0);
  }

  TEST("buffer add delta may fail") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(!buf.add(-1, 0, 42));
    REQUIRE(!buf.add(block + 1, 0, 42));
    REQUIRE(!buf.add(block, -1, 42));
    REQUIRE(!buf.add(block, 10, 42));
    REQUIRE(!buffer {}.set_chunk_size(-1).add(0, 0, 42));
  }

  TEST("buffer add delta and adjust") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.add(block, 0, 42));
    while (buf.adjust()) { }
    REQUIRE(buf.get(block, 0, -1) == 42);
  }

  TEST("buffer add delta twice and adjust") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.add(block, 0, 20));
    REQUIRE(buf.add(block, 0, 22));
    while (buf.adjust()) { }
    REQUIRE(buf.get(block, 0, -1) == 42);
  }

  TEST("buffer set value and add delta and adjust") {
    auto buf   = buffer {};
    auto block = buf.allocate(10);
    REQUIRE(buf.set(block, 0, 20));
    REQUIRE(buf.add(block, 0, 22));
    while (buf.adjust()) { }
    REQUIRE(buf.get(block, 0, -1) == 42);
  }

  TEST("buffer adjust one chunk") {
    auto buf   = buffer {}.set_chunk_size(100);
    auto block = buf.allocate(100);
    for (int i = 0; i < 100; ++i) std::ignore = buf.set(block, i, i);
    REQUIRE(!buf.adjust());
    bool ok = true;
    for (int i = 0; i < 100; ++i)
      ok = ok && buf.get(block, i, -1) == i;
    REQUIRE(ok);
  }

  TEST("buffer adjust one chunk may fail") {
    auto buf = buffer {}.set_chunk_size(-1);
    REQUIRE(!buf.adjust());
  }

  TEST("buffer adjust by chunks") {
    auto buf   = buffer {}.set_chunk_size(10);
    auto block = buf.allocate(100);
    for (int i = 0; i < 100; ++i) std::ignore = buf.set(block, i, i);
    while (buf.adjust()) { }
    bool ok = true;
    for (int i = 0; i < 100; ++i)
      ok = ok && buf.get(block, i, -1) == i;
    REQUIRE(ok);
  }

  TEST("buffer adjust by chunks twice") {
    auto buf   = buffer {}.set_chunk_size(10);
    auto block = buf.allocate(100);
    for (int i = 0; i < 100; ++i) std::ignore = buf.set(block, i, i);
    while (buf.adjust()) { }
    buf.adjust_done();
    for (int i = 0; i < 100; ++i) std::ignore = buf.add(block, i, i);
    while (buf.adjust()) { }
    bool ok = true;
    for (int i = 0; i < 100; ++i)
      ok = ok && buf.get(block, i, -1) == i * 2;
    REQUIRE(ok);
  }

  TEST("buffer allocate two blocks") {
    auto buf = buffer {};
    auto foo = buf.allocate(10);
    auto bar = buf.allocate(10);
    REQUIRE(foo != id_undefined);
    REQUIRE(bar != id_undefined);
    REQUIRE(foo != bar);
  }

  TEST("buffer fill two blocks") {
    auto buf = buffer {};
    auto foo = buf.allocate(10);
    auto bar = buf.allocate(10);
    REQUIRE(buf.set(foo, 0, 42));
    REQUIRE(buf.set(bar, 0, 42));
    while (buf.adjust()) { }
    REQUIRE(buf.get(foo, 0, -1) == 42);
    REQUIRE(buf.get(bar, 0, -1) == 42);
  }

  TEST("buffer get size") {
    REQUIRE(buffer {}.size() == 0);
  }

  TEST("buffer size with 2 blocks") {
    auto buf    = buffer {};
    std::ignore = buf.allocate(20);
    std::ignore = buf.allocate(22);
    REQUIRE(buf.size() == 42);
  }

  TEST("buffer size after deallocate one") {
    auto buf = buffer {};
    REQUIRE(buf.deallocate(buf.allocate(10)));
    REQUIRE(buf.size() == 10);
  }

  TEST("buffer size after deallocate and allocate into one") {
    auto buf = buffer {};
    REQUIRE(buf.deallocate(buf.allocate(10)));
    std::ignore = buf.allocate(10);
    REQUIRE(buf.size() == 10);
  }

  TEST("buffer deallocate efficiency") {
    auto buf    = buffer {};
    std::ignore = buf.allocate(10);
    REQUIRE(buf.deallocate(buf.allocate(10)));
    std::ignore = buf.allocate(10);
    REQUIRE(buf.size() == 20);
  }

  TEST("buffer add delta concurrency") {
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

  TEST("buffer add delta concurrency harder") {
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

  TEST("buffer adjust concurrency") {
    auto buf = buffer {}.set_chunk_size(10);
    auto id  = buf.allocate(10000);
    for (int i = 0; i < 10000; i++) std::ignore = buf.set(id, i, i);

    auto pool = vector<thread> {};
    for (int i = 0; i < 1000; i++)
      pool.emplace_back(
          thread { [&]() { std::ignore = buf.adjust(); } });
    for (auto &t : pool) t.join();

    bool ok = true;
    for (int i = 0; i < 10000; i++)
      ok = ok && buf.get(id, i, -1) == i;

    REQUIRE(ok);
  }

  TEST("int buffer adjust concurrency harder") {
    auto buf = buffer {}.set_chunk_size(1);
    auto id  = buf.allocate(10000);
    for (int i = 0; i < 10000; i++) std::ignore = buf.set(id, i, i);

    auto pool = vector<thread> {};
    for (int i = 0; i < 10000; i++)
      pool.emplace_back(
          thread { [&]() { std::ignore = buf.adjust(); } });
    for (auto &t : pool) t.join();

    bool ok = true;
    for (int i = 0; i < 10000; i++)
      ok = ok && buf.get(id, i, -1) == i;

    REQUIRE(ok);
  }

  TEST("byte buffer adjust concurrency harder") {
    auto buf = byte_buffer {}.set_chunk_size(1);
    auto id  = buf.allocate(10000);
    for (int i = 0; i < 10000; i++)
      std::ignore = buf.set(id, i, static_cast<byte_type>(i % 128));

    auto pool = vector<thread> {};
    for (int i = 0; i < 10000; i++)
      pool.emplace_back(
          thread { [&]() { std::ignore = buf.adjust(); } });
    for (auto &t : pool) t.join();

    bool ok = true;
    for (int i = 0; i < 10000; i++)
      ok = ok &&
           buf.get(id, i, -1) == static_cast<byte_type>(i % 128);

    REQUIRE(ok);
  }

  TEST("buffer create copy") {
    auto foo = buffer {};
    auto id  = foo.allocate(1);
    REQUIRE(foo.set(id, 0, 42));
    auto bar = foo;
    while (bar.adjust()) { }
    REQUIRE(bar.get(id, 0, -1) == 42);
  }

  TEST("buffer create move") {
    auto foo = buffer {};
    auto id  = foo.allocate(1);
    REQUIRE(foo.set(id, 0, 42));
    auto bar = std::move(foo);
    while (bar.adjust()) { }
    REQUIRE(bar.get(id, 0, -1) == 42);
  }

  TEST("buffer assign copy") {
    auto foo = buffer {};
    auto bar = buffer {};
    auto id  = foo.allocate(1);
    REQUIRE(foo.set(id, 0, 42));
    bar = foo;
    while (bar.adjust()) { }
    REQUIRE(bar.get(id, 0, -1) == 42);
  }

  TEST("buffer assign move") {
    auto foo = buffer {};
    auto bar = buffer {};
    auto id  = foo.allocate(1);
    REQUIRE(foo.set(id, 0, 42));
    bar = std::move(foo);
    while (bar.adjust()) { }
    REQUIRE(bar.get(id, 0, -1) == 42);
  }
}
