#include "../../laplace/buffer.h"
#include <kit/threads.h>

#define KIT_TEST_FILE buffer
#include <kit_test/test.h>

TEST("create int buffer") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  BUFFER_DESTROY(buf);
}

TEST("create byte buffer") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int8_t);
  REQUIRE(s == STATUS_OK);
  BUFFER_DESTROY(buf);
}

TEST("buffer init") {
  laplace_status_t s;
  BUFFER_TYPE(int64_t) buf;
  BUFFER_INIT(s, buf, kit_alloc_default());
  REQUIRE(s == STATUS_OK);
  BUFFER_DESTROY(buf);
}

TEST("buffer get chunk size") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  REQUIRE(buf.chunk_size == BUFFER_DEFAULT_CHUNK_SIZE);
  BUFFER_DESTROY(buf);
}

TEST("buffer set chunk size") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_SET_CHUNK_SIZE(buf, 42) == STATUS_OK);
  REQUIRE(buf.chunk_size == 42);
  BUFFER_DESTROY(buf);
}

TEST("buffer set chunk size may fail") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_SET_CHUNK_SIZE(buf, -1) == ERROR_INVALID_CHUNK_SIZE);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(h.id != ID_UNDEFINED);
  BUFFER_DESTROY(buf);
}

TEST("buffer get size") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(h.id != ID_UNDEFINED);
  REQUIRE(BUFFER_SIZE(buf, h) == 10);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate negative size will fail") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, -1);
  REQUIRE(h.id == ID_UNDEFINED);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate zero size will fail") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 0);
  REQUIRE(h.id == ID_UNDEFINED);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate too big will fail") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, PTRDIFF_MAX);
  REQUIRE(h.id == ID_UNDEFINED);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate into") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t a = { .id = 42, .generation = -1 };
  handle_t b;
  BUFFER_ALLOCATE_INTO(b, buf, 10, a);
  REQUIRE(b.id != ID_UNDEFINED);
  REQUIRE(b.generation == 0);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate and allocate into") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t a;
  BUFFER_ALLOCATE(a, buf, 10);
  handle_t b;
  BUFFER_ALLOCATE_INTO(b, buf, 100, a);
  REQUIRE(b.id != ID_UNDEFINED);
  REQUIRE(b.generation == 1);
  BUFFER_DESTROY(buf);
}

TEST("buffer reserve, allocate into and allocate") {
  laplace_status_t s;
  ptrdiff_t const  reserved = 42;
  handle_t const   h        = { .id = 1, .generation = -1 };
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_RESERVE(buf, reserved) == STATUS_OK);
  handle_t a;
  BUFFER_ALLOCATE_INTO(a, buf, 10, h);
  REQUIRE(a.id == h.id);
  handle_t b;
  BUFFER_ALLOCATE(b, buf, 10);
  REQUIRE(b.id == reserved);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate into undefined will fail") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t a = { .id = ID_UNDEFINED, .generation = -1 };
  handle_t b;
  BUFFER_ALLOCATE_INTO(b, buf, 10, a);
  REQUIRE(b.id == ID_UNDEFINED);
  REQUIRE(b.error == ERROR_INVALID_HANDLE_ID);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate into wrong generation will fail") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t a = { .id = 42, .generation = 0 };
  handle_t b;
  BUFFER_ALLOCATE_INTO(b, buf, 10, a);
  REQUIRE(b.id == ID_UNDEFINED);
  REQUIRE(b.error == ERROR_INVALID_HANDLE_GENERATION);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate into invalid size will fail") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t a = { .id = 42, .generation = -1 };
  handle_t b;
  BUFFER_ALLOCATE_INTO(b, buf, -1, a);
  REQUIRE(b.id == ID_UNDEFINED);
  REQUIRE(b.error == ERROR_INVALID_SIZE);
  BUFFER_DESTROY(buf);
}

TEST("buffer reserve and allocate") {
  laplace_status_t s;
  ptrdiff_t const  reserved = 42;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_RESERVE(buf, reserved) == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(h.id == reserved);
  BUFFER_DESTROY(buf);
}

TEST("buffer reserve may fail") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_RESERVE(buf, -1) == ERROR_INVALID_SIZE);
  BUFFER_DESTROY(buf);
}

TEST("buffer deallocate") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) == STATUS_OK);
  BUFFER_DESTROY(buf);
}

TEST("buffer deallocate invalid handle id") {
  laplace_status_t s;
  handle_t         h = { .id = 42, .generation = -1 };
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) == ERROR_INVALID_HANDLE_ID);
  BUFFER_DESTROY(buf);
}

TEST("buffer deallocate invalid handle generation") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  h.generation--;
  REQUIRE(BUFFER_DEALLOCATE(buf, h) ==
          ERROR_INVALID_HANDLE_GENERATION);
  BUFFER_DESTROY(buf);
}

TEST("buffer deallocate twice will fail") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) == STATUS_OK);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) == ERROR_INVALID_HANDLE_ID);
  BUFFER_DESTROY(buf);
}

TEST("buffer reserve, allocate into, deallocate and allocate") {
  laplace_status_t s;
  ptrdiff_t const  reserved = 42;
  handle_t const   h        = { .id = 1, .generation = -1 };
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_RESERVE(buf, reserved) == STATUS_OK);
  handle_t a;
  BUFFER_ALLOCATE_INTO(a, buf, 10, h);
  REQUIRE(a.id == h.id);
  handle_t b;
  BUFFER_ALLOCATE(b, buf, 10);
  REQUIRE(b.id == reserved);
  BUFFER_DESTROY(buf);
}

TEST("buffer get value") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  int64_t x;
  BUFFER_READ(s, buf, h, 0, 1, &x);
  REQUIRE(s == STATUS_OK);
  REQUIRE(x == 0);
  BUFFER_DESTROY(buf);
}

TEST("buffer get invalid id") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t const h = { .id = 0, .generation = -1 };
  int64_t        x;
  BUFFER_READ(s, buf, h, 0, 1, &x);
  REQUIRE(s == ERROR_INVALID_HANDLE_ID);
  BUFFER_DESTROY(buf);
}

TEST("buffer get invalid generation") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  h.generation--;
  int64_t x;
  BUFFER_READ(s, buf, h, 0, 1, &x);
  REQUIRE(s == ERROR_INVALID_HANDLE_GENERATION);
  BUFFER_DESTROY(buf);
}

TEST("buffer get size") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  REQUIRE(BUFFER_SIZE(buf, h) == 1);
  BUFFER_DESTROY(buf);
}

TEST("buffer get invalid index") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  int64_t x;
  BUFFER_READ(s, buf, h, 1, 1, &x);
  REQUIRE(s == ERROR_INVALID_INDEX);
  BUFFER_DESTROY(buf);
}

TEST("buffer set invalid id") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t const h = { .id = 0, .generation = -1 };
  BUFFER_SET(s, buf, h, 0, 42);
  REQUIRE(s == ERROR_INVALID_HANDLE_ID);
  BUFFER_DESTROY(buf);
}

TEST("buffer set invalid generation") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  h.generation--;
  BUFFER_SET(s, buf, h, 0, 42);
  REQUIRE(s == ERROR_INVALID_HANDLE_GENERATION);
  BUFFER_DESTROY(buf);
}

TEST("buffer set value") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  BUFFER_SET(s, buf, h, 0, 42);
  REQUIRE(s == STATUS_OK);
  int64_t x;
  BUFFER_READ(s, buf, h, 0, 1, &x);
  REQUIRE(s == STATUS_OK);
  REQUIRE(x == 0);
  BUFFER_DESTROY(buf);
}

TEST("buffer set value and adjust") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  BUFFER_SET(s, buf, h, 0, 42);
  REQUIRE(s == STATUS_OK);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  int64_t x;
  BUFFER_READ(s, buf, h, 0, 1, &x);
  REQUIRE(s == STATUS_OK);
  REQUIRE(x == 42);
  BUFFER_DESTROY(buf);
}

TEST("buffer set value, deallocate and get value") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t foo, bar;
  BUFFER_ALLOCATE(foo, buf, 1);
  BUFFER_ALLOCATE(bar, buf, 1);
  BUFFER_SET(s, buf, bar, 0, 42);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_DEALLOCATE(buf, foo) == STATUS_OK);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  int64_t x;
  BUFFER_READ(s, buf, bar, 0, 1, &x);
  REQUIRE(s == STATUS_OK);
  REQUIRE(x == 42);
  BUFFER_DESTROY(buf);
}

TEST("buffer set value, deallocate, allocate and get value") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  BUFFER_SET(s, buf, h, 0, 42);
  REQUIRE(s == STATUS_OK);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) == STATUS_OK);
  BUFFER_ALLOCATE(h, buf, 1);
  int64_t x;
  BUFFER_READ(s, buf, h, 0, 1, &x);
  REQUIRE(s == STATUS_OK);
  REQUIRE(x == 0);
  BUFFER_DESTROY(buf);
}

TEST("buffer add delta and adjust") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  BUFFER_ADD(s, buf, h, 0, 42);
  REQUIRE(s == STATUS_OK);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  int64_t x;
  BUFFER_READ(s, buf, h, 0, 1, &x);
  REQUIRE(s == STATUS_OK);
  REQUIRE(x == 42);
  BUFFER_DESTROY(buf);
}

TEST("buffer add delta twice and adjust") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  BUFFER_ADD(s, buf, h, 0, 20);
  REQUIRE(s == STATUS_OK);
  BUFFER_ADD(s, buf, h, 0, 22);
  REQUIRE(s == STATUS_OK);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  int64_t x;
  BUFFER_READ(s, buf, h, 0, 1, &x);
  REQUIRE(s == STATUS_OK);
  REQUIRE(x == 42);
  BUFFER_DESTROY(buf);
}

TEST("buffer set value and add delta and adjust") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  BUFFER_SET(s, buf, h, 0, 20);
  REQUIRE(s == STATUS_OK);
  BUFFER_ADD(s, buf, h, 0, 22);
  REQUIRE(s == STATUS_OK);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  int64_t x;
  BUFFER_READ(s, buf, h, 0, 1, &x);
  REQUIRE(s == STATUS_OK);
  REQUIRE(x == 42);
  BUFFER_DESTROY(buf);
}

TEST("buffer adjust one chunk") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_SET_CHUNK_SIZE(buf, 100) == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 100);
  for (int i = 0; i < 100; ++i) BUFFER_SET(s, buf, h, i, i);
  int running;
  BUFFER_ADJUST(running, buf);
  REQUIRE(!running);
  int ok = 1;
  for (int i = 0; i < 100; ++i) {
    int64_t x;
    BUFFER_READ(s, buf, h, i, 1, &x);
    ok = ok && s == STATUS_OK;
    ok = ok && x == i;
  }
  REQUIRE(ok);
  BUFFER_DESTROY(buf);
}

TEST("buffer adjust by chunks") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_SET_CHUNK_SIZE(buf, 100) == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 100);
  for (int i = 0; i < 100; ++i) BUFFER_SET(s, buf, h, i, i);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  int ok = 1;
  for (int i = 0; i < 100; ++i) {
    int64_t x;
    BUFFER_READ(s, buf, h, i, 1, &x);
    ok = ok && s == STATUS_OK;
    ok = ok && x == i;
  }
  REQUIRE(ok);
  BUFFER_DESTROY(buf);
}

TEST("buffer adjust by chunks twice") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_SET_CHUNK_SIZE(buf, 100) == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 100);
  for (int i = 0; i < 100; ++i) BUFFER_SET(s, buf, h, i, i);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  BUFFER_ADJUST_DONE(buf);
  for (int i = 0; i < 100; ++i) BUFFER_ADD(s, buf, h, i, i);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  int ok = 1;
  for (int i = 0; i < 100; ++i) {
    int64_t x;
    BUFFER_READ(s, buf, h, i, 1, &x);
    ok = ok && s == STATUS_OK;
    ok = ok && x == i * 2;
  }
  REQUIRE(ok);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate two blocks") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t foo, bar;
  BUFFER_ALLOCATE(foo, buf, 10);
  BUFFER_ALLOCATE(bar, buf, 10);
  REQUIRE(foo.id != ID_UNDEFINED);
  REQUIRE(bar.id != ID_UNDEFINED);
  REQUIRE(foo.id != bar.id);
  BUFFER_DESTROY(buf);
}

TEST("buffer fill two blocks") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t foo, bar;
  BUFFER_ALLOCATE(foo, buf, 10);
  BUFFER_ALLOCATE(bar, buf, 10);
  BUFFER_SET(s, buf, foo, 0, 42);
  REQUIRE(s == STATUS_OK);
  BUFFER_SET(s, buf, bar, 0, 42);
  REQUIRE(s == STATUS_OK);
  BUFFER_ADJUST_LOOP(buf);
  int64_t x;
  BUFFER_READ(s, buf, foo, 0, 1, &x);
  REQUIRE(s == STATUS_OK);
  REQUIRE(x == 42);
  BUFFER_READ(s, buf, bar, 0, 1, &x);
  REQUIRE(s == STATUS_OK);
  REQUIRE(x == 42);
  BUFFER_DESTROY(buf);
}

TEST("buffer size with 2 blocks") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 20);
  BUFFER_ALLOCATE(h, buf, 22);
  REQUIRE(buf.data.size == 42);
  BUFFER_DESTROY(buf);
}

TEST("buffer size after deallocate one") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) == STATUS_OK);
  REQUIRE(buf.data.size == 10);
  BUFFER_DESTROY(buf);
}

TEST("buffer size after deallocate and allocate into one") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) == STATUS_OK);
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(buf.data.size == 10);
  BUFFER_DESTROY(buf);
}

TEST("buffer deallocate efficiency") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t a, b;
  BUFFER_ALLOCATE(a, buf, 10);
  BUFFER_ALLOCATE(b, buf, 10);
  REQUIRE(BUFFER_DEALLOCATE(buf, b) == STATUS_OK);
  BUFFER_ALLOCATE(b, buf, 10);
  REQUIRE(buf.data.size == 20);
  BUFFER_DESTROY(buf);
}

TEST("buffer reallocate") {
  laplace_status_t s;
  BUFFER_CREATE(s, buf, int64_t);
  REQUIRE(s == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(buf.blocks.values[h.id].size == 10);
  BUFFER_REALLOCATE(s, buf, 20, h);
  REQUIRE(s == STATUS_OK);
  REQUIRE(buf.blocks.values[h.id].size == 20);
  BUFFER_DESTROY(buf);
}

typedef BUFFER_TYPE(int64_t) test_buffer_int_t;

typedef struct {
  test_buffer_int_t buf;
  handle_t          h;
} test_int_buffer_data_t;

static int test_int_add_delta(void *p) {
  test_int_buffer_data_t *data = (test_int_buffer_data_t *) p;
  laplace_status_t        s;
  for (int j = 0; j < 100; j++) {
    BUFFER_ADD(s, data->buf, data->h, 0, 1);
    BUFFER_ADD(s, data->buf, data->h, 0, -1);
  }
  BUFFER_ADD(s, data->buf, data->h, 0, 1);
  return 0;
}

TEST("buffer add delta concurrency") {
  enum { THREAD_COUNT = 100 };
  laplace_status_t       s;
  test_int_buffer_data_t data;
  BUFFER_INIT(s, data.buf, kit_alloc_default());
  REQUIRE(s == STATUS_OK);
  BUFFER_ALLOCATE(data.h, data.buf, 1);
  thrd_t pool[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
    thrd_create(pool + i, test_int_add_delta, &data);
  for (int i = 0; i < THREAD_COUNT; i++) thrd_join(pool[i], NULL);
  BUFFER_ADJUST_LOOP(data.buf);
  int64_t x;
  BUFFER_READ(s, data.buf, data.h, 0, 1, &x);
  REQUIRE(s == STATUS_OK);
  REQUIRE(x == THREAD_COUNT);
  BUFFER_DESTROY(data.buf);
}

TEST("buffer add delta concurrency harder") {
  enum { THREAD_COUNT = 1000 };
  laplace_status_t       s;
  test_int_buffer_data_t data;
  BUFFER_INIT(s, data.buf, kit_alloc_default());
  REQUIRE(s == STATUS_OK);
  BUFFER_ALLOCATE(data.h, data.buf, 1);
  thrd_t pool[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
    thrd_create(pool + i, test_int_add_delta, &data);
  for (int i = 0; i < THREAD_COUNT; i++) thrd_join(pool[i], NULL);
  BUFFER_ADJUST_LOOP(data.buf);
  int64_t x;
  BUFFER_READ(s, data.buf, data.h, 0, 1, &x);
  REQUIRE(s == STATUS_OK);
  REQUIRE(x == THREAD_COUNT);
  BUFFER_DESTROY(data.buf);
}

static int test_int_adjust(void *p) {
  test_int_buffer_data_t *data = (test_int_buffer_data_t *) p;
  int                     _;
  BUFFER_ADJUST(_, data->buf);
  (void) _;
  return 0;
}

TEST("buffer adjust concurrency") {
  enum { THREAD_COUNT = 1000, DATA_SIZE = 10000 };
  laplace_status_t       s;
  test_int_buffer_data_t data;
  BUFFER_INIT(s, data.buf, kit_alloc_default());
  REQUIRE(s == STATUS_OK);
  BUFFER_SET_CHUNK_SIZE(data.buf, 10);
  BUFFER_ALLOCATE(data.h, data.buf, DATA_SIZE);
  for (int i = 0; i < DATA_SIZE; i++)
    BUFFER_SET(s, data.buf, data.h, i, i);
  thrd_t pool[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
    thrd_create(pool + i, test_int_adjust, &data);
  for (int i = 0; i < THREAD_COUNT; i++) thrd_join(pool[i], NULL);
  int ok = 1;
  for (int i = 0; i < DATA_SIZE; i++) {
    int64_t x;
    BUFFER_READ(s, data.buf, data.h, i, 1, &x);
    ok = ok && s == STATUS_OK;
    ok = ok && x == i;
  }
  REQUIRE(ok);
  BUFFER_DESTROY(data.buf);
}

TEST("int buffer adjust concurrency harder") {
  enum { THREAD_COUNT = 10000, DATA_SIZE = 10000 };
  laplace_status_t       s;
  test_int_buffer_data_t data;
  BUFFER_INIT(s, data.buf, kit_alloc_default());
  REQUIRE(s == STATUS_OK);
  BUFFER_SET_CHUNK_SIZE(data.buf, 1);
  BUFFER_ALLOCATE(data.h, data.buf, DATA_SIZE);
  for (int i = 0; i < DATA_SIZE; i++)
    BUFFER_SET(s, data.buf, data.h, i, i);
  thrd_t pool[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
    thrd_create(pool + i, test_int_adjust, &data);
  for (int i = 0; i < THREAD_COUNT; i++) thrd_join(pool[i], NULL);
  int ok = 1;
  for (int i = 0; i < DATA_SIZE; i++) {
    int64_t x;
    BUFFER_READ(s, data.buf, data.h, i, 1, &x);
    ok = ok && s == STATUS_OK;
    ok = ok && x == i;
  }
  REQUIRE(ok);
  BUFFER_DESTROY(data.buf);
}

typedef BUFFER_TYPE(int8_t) test_buffer_byte_t;

typedef struct {
  test_buffer_byte_t buf;
  handle_t           h;
} test_byte_buffer_data_t;

static int test_byte_adjust(void *p) {
  test_byte_buffer_data_t *data = (test_byte_buffer_data_t *) p;
  int                      _;
  BUFFER_ADJUST(_, data->buf);
  (void) _;
  return 0;
}

TEST("byte buffer adjust concurrency harder") {
  enum { THREAD_COUNT = 10000, DATA_SIZE = 10000 };
  test_byte_buffer_data_t data;
  laplace_status_t        s;
  BUFFER_INIT(s, data.buf, kit_alloc_default());
  REQUIRE(s == STATUS_OK);
  BUFFER_SET_CHUNK_SIZE(data.buf, 1);
  BUFFER_ALLOCATE(data.h, data.buf, DATA_SIZE);
  for (int i = 0; i < DATA_SIZE; i++)
    BUFFER_SET(s, data.buf, data.h, i, (int8_t) (i % 128));
  thrd_t pool[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
    thrd_create(pool + i, test_byte_adjust, &data);
  for (int i = 0; i < THREAD_COUNT; i++) thrd_join(pool[i], NULL);
  int ok = 1;
  for (int i = 0; i < DATA_SIZE; i++) {
    int8_t x;
    BUFFER_READ(s, data.buf, data.h, i, 1, &x);
    ok = ok && s == STATUS_OK;
    ok = ok && x == (int8_t) (i % 128);
  }
  REQUIRE(ok);
  BUFFER_DESTROY(data.buf);
}

typedef struct {
  ATOMIC(int) done;
  test_buffer_int_t buf;
  handle_t          h;
} read_concurrency_data_t;

static int test_read_concurrency_run(void *p) {
  read_concurrency_data_t *data = (read_concurrency_data_t *) p;

  while (atomic_load_explicit(&data->done, memory_order_acquire) ==
         0) {
    ptrdiff_t size = BUFFER_SIZE(data->buf, data->h);
    int64_t   x;
    for (ptrdiff_t i = 0; i < size; i++) {
      laplace_status_t s;
      BUFFER_READ(s, data->buf, data->h, i, 1, &x);
    }
    thrd_yield();
  }

  return 0;
}

TEST("buffer read concurrency") {
  int ok = 1;

  for (ptrdiff_t k = 0; k < 500; k++) {
    read_concurrency_data_t data;
    atomic_store_explicit(&data.done, 0, memory_order_relaxed);

    laplace_status_t s;
    BUFFER_INIT(s, data.buf, kit_alloc_default());
    ok = ok && s == STATUS_OK;

    data.h.id         = 0;
    data.h.generation = 0;

    thrd_t t;
    thrd_create(&t, test_read_concurrency_run, &data);

    struct timespec delay = { .tv_sec = 0, .tv_nsec = 1000000 };

    ok = ok && thrd_sleep(&delay, NULL) == thrd_success;

    handle_t h = { .id = 0, .generation = -1 };
    handle_t g;

    BUFFER_ALLOCATE_INTO(h, data.buf, 1, h);
    ok = ok && h.id != ID_UNDEFINED;

    ok = ok && thrd_sleep(&delay, NULL) == thrd_success;

    BUFFER_REALLOCATE(s, data.buf, 2, h);
    ok = ok && s == STATUS_OK;

    BUFFER_ALLOCATE(g, data.buf, 1);
    ok = ok && g.id != ID_UNDEFINED;

    ok = ok && thrd_sleep(&delay, NULL) == thrd_success;

    BUFFER_REALLOCATE(s, data.buf, 1, h);
    ok = ok && s == STATUS_OK;

    BUFFER_ALLOCATE(g, data.buf, 1);
    ok = ok && g.id != ID_UNDEFINED;

    ok = ok && thrd_sleep(&delay, NULL) == thrd_success;

    BUFFER_REALLOCATE(s, data.buf, 4, h);
    ok = ok && s == STATUS_OK;

    BUFFER_ALLOCATE(g, data.buf, 1);
    ok = ok && g.id != ID_UNDEFINED;

    ok = ok && thrd_sleep(&delay, NULL) == thrd_success;

    BUFFER_REALLOCATE(s, data.buf, 1, h);
    ok = ok && s == STATUS_OK;

    BUFFER_ALLOCATE(g, data.buf, 1);
    ok = ok && g.id != ID_UNDEFINED;

    ok = ok && thrd_sleep(&delay, NULL) == thrd_success;

    BUFFER_REALLOCATE(s, data.buf, 8, h);
    ok = ok && s == STATUS_OK;

    BUFFER_ALLOCATE(g, data.buf, 1);
    ok = ok && g.id != ID_UNDEFINED;

    ok = ok && thrd_sleep(&delay, NULL) == thrd_success;

    BUFFER_REALLOCATE(s, data.buf, 1, h);
    ok = ok && s == STATUS_OK;

    BUFFER_ALLOCATE(g, data.buf, 1);
    ok = ok && g.id != ID_UNDEFINED;

    ok = ok && thrd_sleep(&delay, NULL) == thrd_success;

    BUFFER_REALLOCATE(s, data.buf, 16, h);
    ok = ok && s == STATUS_OK;

    BUFFER_ALLOCATE(g, data.buf, 1);
    ok = ok && g.id != ID_UNDEFINED;

    ok = ok && thrd_sleep(&delay, NULL) == thrd_success;

    atomic_store_explicit(&data.done, 1, memory_order_release);
    thrd_join(t, NULL);

    BUFFER_DESTROY(data.buf);
  }

  REQUIRE(ok);
}
