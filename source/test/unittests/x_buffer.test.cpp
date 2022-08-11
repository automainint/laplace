#include "../../laplace/x_buffer.h"
#include <limits>
#include <thread>

#define KIT_TEST_FILE x_buffer
#include <kit_test/test.h>

using std::numeric_limits, std::thread, std::vector;

TEST("x create int buffer") {
  REQUIRE(!laplace::buffer {}.error());
}

TEST("x create byte buffer") {
  REQUIRE(!laplace::byte_buffer {}.error());
}

TEST("x buffer get chunk size") {
  REQUIRE(laplace::buffer {}.chunk_size() ==
          laplace::buffer::default_chunk_size);
}

TEST("x buffer set chunk size") {
  REQUIRE(laplace::buffer {}.set_chunk_size(10).chunk_size() == 10);
}

TEST("x buffer set chunk size may fail") {
  REQUIRE(laplace::buffer {}.set_chunk_size(-1).error());
}

TEST("x buffer propagate error") {
  REQUIRE(laplace::buffer {}
              .set_chunk_size(-1)
              .set_chunk_size(10)
              .error());
}

TEST("x buffer allocate") {
  REQUIRE(laplace::buffer {}.allocate(10) != laplace::id_undefined);
}

TEST("x buffer allocate may fail") {
  REQUIRE(laplace::buffer {}.allocate(-1) == laplace::id_undefined);
  REQUIRE(laplace::buffer {}.allocate(0) == laplace::id_undefined);
  REQUIRE(
      laplace::buffer {}.allocate(numeric_limits<ptrdiff_t>::max()) ==
      laplace::id_undefined);
  REQUIRE(laplace::buffer {}.set_chunk_size(-1).allocate(10) ==
          laplace::id_undefined);
}

TEST("x buffer allocate into") {
  REQUIRE(laplace::buffer {}.allocate_into(42, 10));
}

TEST("x buffer allocate into may fail") {
  REQUIRE(
      !laplace::buffer {}.allocate_into(laplace::id_undefined, 10));
  REQUIRE(!laplace::buffer {}.allocate_into(42, -1));
  REQUIRE(!laplace::buffer {}.allocate_into(42, 0));
  REQUIRE(!laplace::buffer {}.allocate_into(
      42, numeric_limits<ptrdiff_t>::max()));
  REQUIRE(
      !laplace::buffer {}.set_chunk_size(-1).allocate_into(42, 10));
}

TEST("x buffer allocate and allocate into") {
  auto buf = laplace::buffer {};
  auto id  = buf.allocate(10);
  REQUIRE(buf.allocate_into(id, 100));
}

TEST("x buffer reserve and allocate") {
  auto      buf      = laplace::buffer {};
  ptrdiff_t reserved = 42;
  REQUIRE(buf.reserve(reserved));
  REQUIRE(buf.allocate(10) == reserved);
}

TEST("x buffer reserve may fail") {
  REQUIRE(!laplace::buffer {}.reserve(-1));
}

TEST("x buffer reserve, allocate into and allocate") {
  auto      buf      = laplace::buffer {};
  ptrdiff_t reserved = 42;
  ptrdiff_t id       = 1;
  REQUIRE(buf.reserve(reserved));
  REQUIRE(buf.allocate_into(id, 10));
  REQUIRE(buf.allocate(10) == reserved);
}

TEST("x buffer reserve, deallocate and allocate") {
  auto      buf      = laplace::buffer {};
  ptrdiff_t reserved = 42;
  ptrdiff_t id       = 1;
  REQUIRE(buf.reserve(reserved));
  REQUIRE(buf.allocate_into(id, 10));
  REQUIRE(buf.deallocate(id));
  REQUIRE(buf.allocate(10) == reserved);
}

TEST("x buffer deallocate") {
  auto buf = laplace::buffer {};
  REQUIRE(buf.deallocate(buf.allocate(10)));
}

TEST("x buffer deallocate invalid") {
  REQUIRE(!laplace::buffer {}.deallocate(-1));
  REQUIRE(!laplace::buffer {}.deallocate(42));
}

TEST("x buffer deallocate may fail") {
  auto buf = laplace::buffer {}.set_chunk_size(-1);
  REQUIRE(!buf.deallocate(0));
}

TEST("x buffer deallocate twice will fail") {
  auto buf   = laplace::buffer {};
  auto block = buf.allocate(10);
  REQUIRE(buf.deallocate(block));
  REQUIRE(!buf.deallocate(block));
}

TEST("x buffer get value") {
  auto buf   = laplace::buffer {};
  auto block = buf.allocate(10);
  REQUIRE(buf.get(block, 0, -1) == 0);
}

TEST("x buffer get value may fail") {
  auto buf   = laplace::buffer {};
  auto block = buf.allocate(10);
  REQUIRE(buf.get(-1, 0, 42) == 42);
  REQUIRE(buf.get(block + 1, 0, 42) == 42);
  REQUIRE(buf.get(block, -1, 42) == 42);
  REQUIRE(buf.get(block, 10, 42) == 42);
  REQUIRE(laplace::buffer {}.set_chunk_size(-1).get(0, 0, 42) == 42);
}

TEST("x buffer set value") {
  auto buf   = laplace::buffer {};
  auto block = buf.allocate(10);
  REQUIRE(buf.set(block, 0, 42));
  REQUIRE(buf.get(block, 0, -1) == 0);
}

TEST("x buffer set value may fail") {
  auto buf   = laplace::buffer {};
  auto block = buf.allocate(10);
  REQUIRE(!buf.set(-1, 0, 42));
  REQUIRE(!buf.set(block + 1, 0, 42));
  REQUIRE(!buf.set(block, -1, 42));
  REQUIRE(!buf.set(block, 10, 42));
  REQUIRE(!laplace::buffer {}.set_chunk_size(-1).set(0, 0, 42));
}

TEST("x buffer set value and adjust") {
  auto buf   = laplace::buffer {};
  auto block = buf.allocate(10);
  REQUIRE(buf.set(block, 0, 42));
  while (buf.adjust()) { }
  REQUIRE(buf.get(block, 0, -1) == 42);
}

TEST("x buffer set value, deallocate and get value") {
  auto buf = laplace::buffer {};
  auto foo = buf.allocate(1);
  auto bar = buf.allocate(1);
  REQUIRE(buf.set(bar, 0, 42));
  REQUIRE(buf.deallocate(foo));
  while (buf.adjust()) { }
  REQUIRE(buf.get(bar, 0, -1) == 42);
}

TEST("x buffer set value, deallocate, allocate and get value") {
  auto buf = laplace::buffer {};
  auto id  = buf.allocate(1);
  REQUIRE(buf.set(id, 0, 42));
  while (buf.adjust()) { }
  REQUIRE(buf.deallocate(id));
  id = buf.allocate(1);
  REQUIRE(buf.get(id, 0, -1) == 0);
}

TEST("x buffer add delta may fail") {
  auto buf   = laplace::buffer {};
  auto block = buf.allocate(10);
  REQUIRE(!buf.add(-1, 0, 42));
  REQUIRE(!buf.add(block + 1, 0, 42));
  REQUIRE(!buf.add(block, -1, 42));
  REQUIRE(!buf.add(block, 10, 42));
  REQUIRE(!laplace::buffer {}.set_chunk_size(-1).add(0, 0, 42));
}

TEST("x buffer add delta and adjust") {
  auto buf   = laplace::buffer {};
  auto block = buf.allocate(10);
  REQUIRE(buf.add(block, 0, 42));
  while (buf.adjust()) { }
  REQUIRE(buf.get(block, 0, -1) == 42);
}

TEST("x buffer add delta twice and adjust") {
  auto buf   = laplace::buffer {};
  auto block = buf.allocate(10);
  REQUIRE(buf.add(block, 0, 20));
  REQUIRE(buf.add(block, 0, 22));
  while (buf.adjust()) { }
  REQUIRE(buf.get(block, 0, -1) == 42);
}

TEST("x buffer set value and add delta and adjust") {
  auto buf   = laplace::buffer {};
  auto block = buf.allocate(10);
  REQUIRE(buf.set(block, 0, 20));
  REQUIRE(buf.add(block, 0, 22));
  while (buf.adjust()) { }
  REQUIRE(buf.get(block, 0, -1) == 42);
}

TEST("x buffer adjust one chunk") {
  auto buf   = laplace::buffer {}.set_chunk_size(100);
  auto block = buf.allocate(100);
  for (int i = 0; i < 100; ++i) std::ignore = buf.set(block, i, i);
  REQUIRE(!buf.adjust());
  bool ok = true;
  for (int i = 0; i < 100; ++i) ok = ok && buf.get(block, i, -1) == i;
  REQUIRE(ok);
}

TEST("x buffer adjust one chunk may fail") {
  auto buf = laplace::buffer {}.set_chunk_size(-1);
  REQUIRE(!buf.adjust());
}

TEST("x buffer adjust by chunks") {
  auto buf   = laplace::buffer {}.set_chunk_size(10);
  auto block = buf.allocate(100);
  for (int i = 0; i < 100; ++i) std::ignore = buf.set(block, i, i);
  while (buf.adjust()) { }
  bool ok = true;
  for (int i = 0; i < 100; ++i) ok = ok && buf.get(block, i, -1) == i;
  REQUIRE(ok);
}

TEST("x buffer adjust by chunks twice") {
  auto buf   = laplace::buffer {}.set_chunk_size(10);
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

TEST("x buffer allocate two blocks") {
  auto buf = laplace::buffer {};
  auto foo = buf.allocate(10);
  auto bar = buf.allocate(10);
  REQUIRE(foo != laplace::id_undefined);
  REQUIRE(bar != laplace::id_undefined);
  REQUIRE(foo != bar);
}

TEST("x buffer fill two blocks") {
  auto buf = laplace::buffer {};
  auto foo = buf.allocate(10);
  auto bar = buf.allocate(10);
  REQUIRE(buf.set(foo, 0, 42));
  REQUIRE(buf.set(bar, 0, 42));
  while (buf.adjust()) { }
  REQUIRE(buf.get(foo, 0, -1) == 42);
  REQUIRE(buf.get(bar, 0, -1) == 42);
}

TEST("x buffer get size") {
  REQUIRE(laplace::buffer {}.size() == 0);
}

TEST("x buffer size with 2 blocks") {
  auto buf    = laplace::buffer {};
  std::ignore = buf.allocate(20);
  std::ignore = buf.allocate(22);
  REQUIRE(buf.size() == 42);
}

TEST("x buffer size after deallocate one") {
  auto buf = laplace::buffer {};
  REQUIRE(buf.deallocate(buf.allocate(10)));
  REQUIRE(buf.size() == 10);
}

TEST("x buffer size after deallocate and allocate into one") {
  auto buf = laplace::buffer {};
  REQUIRE(buf.deallocate(buf.allocate(10)));
  std::ignore = buf.allocate(10);
  REQUIRE(buf.size() == 10);
}

TEST("x buffer deallocate efficiency") {
  auto buf    = laplace::buffer {};
  std::ignore = buf.allocate(10);
  REQUIRE(buf.deallocate(buf.allocate(10)));
  std::ignore = buf.allocate(10);
  REQUIRE(buf.size() == 20);
}

TEST("x buffer add delta concurrency") {
  auto buf = laplace::buffer {};
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

TEST("x buffer add delta concurrency harder") {
  auto buf = laplace::buffer {};
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

TEST("x buffer adjust concurrency") {
  auto buf = laplace::buffer {}.set_chunk_size(10);
  auto id  = buf.allocate(10000);
  for (int i = 0; i < 10000; i++) std::ignore = buf.set(id, i, i);

  auto pool = vector<thread> {};
  for (int i = 0; i < 1000; i++)
    pool.emplace_back(
        thread { [&]() { std::ignore = buf.adjust(); } });
  for (auto &t : pool) t.join();

  bool ok = true;
  for (int i = 0; i < 10000; i++) ok = ok && buf.get(id, i, -1) == i;

  REQUIRE(ok);
}

TEST("x int buffer adjust concurrency harder") {
  auto buf = laplace::buffer {}.set_chunk_size(1);
  auto id  = buf.allocate(10000);
  for (int i = 0; i < 10000; i++) std::ignore = buf.set(id, i, i);

  auto pool = vector<thread> {};
  for (int i = 0; i < 10000; i++)
    pool.emplace_back(
        thread { [&]() { std::ignore = buf.adjust(); } });
  for (auto &t : pool) t.join();

  bool ok = true;
  for (int i = 0; i < 10000; i++) ok = ok && buf.get(id, i, -1) == i;

  REQUIRE(ok);
}

TEST("x byte buffer adjust concurrency harder") {
  auto buf = laplace::byte_buffer {}.set_chunk_size(1);
  auto id  = buf.allocate(10000);
  for (int i = 0; i < 10000; i++)
    std::ignore = buf.set(id, i,
                          static_cast<laplace::byte_type>(i % 128));

  auto pool = vector<thread> {};
  for (int i = 0; i < 10000; i++)
    pool.emplace_back(
        thread { [&]() { std::ignore = buf.adjust(); } });
  for (auto &t : pool) t.join();

  bool ok = true;
  for (int i = 0; i < 10000; i++)
    ok = ok && buf.get(id, i, -1) ==
                   static_cast<laplace::byte_type>(i % 128);

  REQUIRE(ok);
}

TEST("x buffer create copy") {
  auto foo = laplace::buffer {};
  auto id  = foo.allocate(1);
  REQUIRE(foo.set(id, 0, 42));
  auto bar = foo;
  while (bar.adjust()) { }
  REQUIRE(bar.get(id, 0, -1) == 42);
}

TEST("x buffer create move") {
  auto foo = laplace::buffer {};
  auto id  = foo.allocate(1);
  REQUIRE(foo.set(id, 0, 42));
  auto bar = std::move(foo);
  while (bar.adjust()) { }
  REQUIRE(bar.get(id, 0, -1) == 42);
}

TEST("x buffer assign copy") {
  auto foo = laplace::buffer {};
  auto bar = laplace::buffer {};
  auto id  = foo.allocate(1);
  REQUIRE(foo.set(id, 0, 42));
  bar = foo;
  while (bar.adjust()) { }
  REQUIRE(bar.get(id, 0, -1) == 42);
}

TEST("x buffer assign move") {
  auto foo = laplace::buffer {};
  auto bar = laplace::buffer {};
  auto id  = foo.allocate(1);
  REQUIRE(foo.set(id, 0, 42));
  bar = std::move(foo);
  while (bar.adjust()) { }
  REQUIRE(bar.get(id, 0, -1) == 42);
}
