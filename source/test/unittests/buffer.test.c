#include "../../laplace/buffer.h"

#define KIT_TEST_FILE buffer
#include <kit_test/test.h>

#ifdef __unix__
#  include <pthread.h>
#endif

TEST("create int buffer") {
  BUFFER_CREATE(buf, int64_t);
  BUFFER_DESTROY(buf);
}

TEST("create byte buffer") {
  BUFFER_CREATE(buf, int8_t);
  BUFFER_DESTROY(buf);
}

TEST("buffer init") {
  BUFFER_TYPE(int64_t) buf;
  BUFFER_INIT(buf, kit_alloc_default());
  BUFFER_DESTROY(buf);
}

TEST("buffer get chunk size") {
  BUFFER_CREATE(buf, int64_t);
  REQUIRE(buf.chunk_size ==
          BUFFER_DEFAULT_CHUNK_SIZE / sizeof(int64_t));
  BUFFER_DESTROY(buf);
}

TEST("buffer set chunk size") {
  BUFFER_CREATE(buf, int64_t);
  REQUIRE(BUFFER_SET_CHUNK_SIZE(buf, 42) == STATUS_OK);
  REQUIRE(buf.chunk_size == 42);
  BUFFER_DESTROY(buf);
}

TEST("buffer set chunk size may fail") {
  BUFFER_CREATE(buf, int64_t);
  REQUIRE(BUFFER_SET_CHUNK_SIZE(buf, -1) ==
          BUFFER_ERROR_INVALID_CHUNK_SIZE);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(h.id != ID_UNDEFINED);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate negative size will fail") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, -1);
  REQUIRE(h.id == ID_UNDEFINED);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate zero size will fail") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 0);
  REQUIRE(h.id == ID_UNDEFINED);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate too big will fail") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, PTRDIFF_MAX);
  REQUIRE(h.id == ID_UNDEFINED);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate into") {
  BUFFER_CREATE(buf, int64_t);
  handle_t a = { .id = 42, .generation = -1 };
  handle_t b;
  BUFFER_ALLOCATE_INTO(b, buf, 10, a);
  REQUIRE(b.id != ID_UNDEFINED);
  REQUIRE(b.generation == 0);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate and allocate into") {
  BUFFER_CREATE(buf, int64_t);
  handle_t a;
  BUFFER_ALLOCATE(a, buf, 10);
  handle_t b;
  BUFFER_ALLOCATE_INTO(b, buf, 100, a);
  REQUIRE(b.id != ID_UNDEFINED);
  REQUIRE(b.generation == 1);
  BUFFER_DESTROY(buf);
}

TEST("buffer reserve, allocate into and allocate") {
  ptrdiff_t const reserved = 42;
  handle_t const  h        = { .id = 1, .generation = -1 };
  BUFFER_CREATE(buf, int64_t);
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
  BUFFER_CREATE(buf, int64_t);
  handle_t a = { .id = ID_UNDEFINED, .generation = -1 };
  handle_t b;
  BUFFER_ALLOCATE_INTO(b, buf, 10, a);
  REQUIRE(b.id == ID_UNDEFINED);
  REQUIRE(b.error == BUFFER_ERROR_INVALID_HANDLE_ID);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate into wrong generation will fail") {
  BUFFER_CREATE(buf, int64_t);
  handle_t a = { .id = 42, .generation = 0 };
  handle_t b;
  BUFFER_ALLOCATE_INTO(b, buf, 10, a);
  REQUIRE(b.id == ID_UNDEFINED);
  REQUIRE(b.error == BUFFER_ERROR_INVALID_HANDLE_GENERATION);
  BUFFER_DESTROY(buf);
}

TEST("buffer allocate into invalid size will fail") {
  BUFFER_CREATE(buf, int64_t);
  handle_t a = { .id = 42, .generation = -1 };
  handle_t b;
  BUFFER_ALLOCATE_INTO(b, buf, -1, a);
  REQUIRE(b.id == ID_UNDEFINED);
  REQUIRE(b.error == BUFFER_ERROR_INVALID_SIZE);
  BUFFER_DESTROY(buf);
}

TEST("buffer reserve and allocate") {
  ptrdiff_t const reserved = 42;
  BUFFER_CREATE(buf, int64_t);
  REQUIRE(BUFFER_RESERVE(buf, reserved) == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(h.id == reserved);
  BUFFER_DESTROY(buf);
}

TEST("buffer reserve may fail") {
  BUFFER_CREATE(buf, int64_t);
  REQUIRE(BUFFER_RESERVE(buf, -1) == BUFFER_ERROR_INVALID_SIZE);
  BUFFER_DESTROY(buf);
}

TEST("buffer deallocate") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) == STATUS_OK);
  BUFFER_DESTROY(buf);
}

TEST("buffer deallocate invalid handle id") {
  handle_t h = { .id = 42, .generation = -1 };
  BUFFER_CREATE(buf, int64_t);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) ==
          BUFFER_ERROR_INVALID_HANDLE_ID);
  BUFFER_DESTROY(buf);
}

TEST("buffer deallocate invalid handle generation") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  h.generation--;
  REQUIRE(BUFFER_DEALLOCATE(buf, h) ==
          BUFFER_ERROR_INVALID_HANDLE_GENERATION);
  BUFFER_DESTROY(buf);
}

TEST("buffer deallocate twice will fail") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) == STATUS_OK);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) ==
          BUFFER_ERROR_INVALID_HANDLE_ID);
  BUFFER_DESTROY(buf);
}

TEST("buffer reserve, allocate into, deallocate and allocate") {
  ptrdiff_t const reserved = 42;
  handle_t const  h        = { .id = 1, .generation = -1 };
  BUFFER_CREATE(buf, int64_t);
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
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  int64_t x = BUFFER_GET(buf, h, 0, -1);
  REQUIRE(x == 0);
  BUFFER_DESTROY(buf);
}

TEST("buffer get invalid id") {
  BUFFER_CREATE(buf, int64_t);
  handle_t const h = { .id = 0, .generation = -1 };
  REQUIRE(BUFFER_GET(buf, h, 0, -1) == -1);
  BUFFER_DESTROY(buf);
}

TEST("buffer get invalid generation") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  h.generation--;
  REQUIRE(BUFFER_GET(buf, h, 0, -1) == -1);
  BUFFER_DESTROY(buf);
}

TEST("buffer get value") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  REQUIRE(BUFFER_GET(buf, h, 1, -1) == -1);
  BUFFER_DESTROY(buf);
}

TEST("buffer set invalid id") {
  BUFFER_CREATE(buf, int64_t);
  handle_t const h = { .id = 0, .generation = -1 };
  status_t       s;
  BUFFER_SET(s, buf, h, 0, 42);
  REQUIRE(s == BUFFER_ERROR_INVALID_HANDLE_ID);
  BUFFER_DESTROY(buf);
}

TEST("buffer set invalid generation") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  h.generation--;
  status_t s;
  BUFFER_SET(s, buf, h, 0, 42);
  REQUIRE(s == BUFFER_ERROR_INVALID_HANDLE_GENERATION);
  BUFFER_DESTROY(buf);
}

TEST("buffer set value") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  status_t s;
  BUFFER_SET(s, buf, h, 0, 42);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_GET(buf, h, 0, -1) == 0);
  BUFFER_DESTROY(buf);
}

TEST("buffer set value and adjust") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  status_t s;
  BUFFER_SET(s, buf, h, 0, 42);
  REQUIRE(s == STATUS_OK);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  REQUIRE(BUFFER_GET(buf, h, 0, -1) == 42);
}

TEST("buffer set value, deallocate and get value") {
  BUFFER_CREATE(buf, int64_t);
  handle_t foo, bar;
  BUFFER_ALLOCATE(foo, buf, 1);
  BUFFER_ALLOCATE(bar, buf, 1);
  status_t s;
  BUFFER_SET(s, buf, bar, 0, 42);
  REQUIRE(s == STATUS_OK);
  REQUIRE(BUFFER_DEALLOCATE(buf, foo) == STATUS_OK);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  REQUIRE(BUFFER_GET(buf, bar, 0, -1) == 42);
}

TEST("buffer set value, deallocate, allocate and get value") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 1);
  status_t s;
  BUFFER_SET(s, buf, h, 0, 42);
  REQUIRE(s == STATUS_OK);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) == STATUS_OK);
  BUFFER_ALLOCATE(h, buf, 1);
  REQUIRE(BUFFER_GET(buf, h, 0, -1) == 0);
}

TEST("buffer add delta and adjust") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  status_t s;
  BUFFER_ADD(s, buf, h, 0, 42);
  REQUIRE(s == STATUS_OK);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  REQUIRE(BUFFER_GET(buf, h, 0, -1) == 42);
}

TEST("buffer add delta twice and adjust") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  status_t s;
  BUFFER_ADD(s, buf, h, 0, 20);
  REQUIRE(s == STATUS_OK);
  BUFFER_ADD(s, buf, h, 0, 22);
  REQUIRE(s == STATUS_OK);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  REQUIRE(BUFFER_GET(buf, h, 0, -1) == 42);
}

TEST("buffer set value and add delta and adjust") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  status_t s;
  BUFFER_SET(s, buf, h, 0, 20);
  REQUIRE(s == STATUS_OK);
  BUFFER_ADD(s, buf, h, 0, 22);
  REQUIRE(s == STATUS_OK);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  REQUIRE(BUFFER_GET(buf, h, 0, -1) == 42);
}

TEST("buffer adjust one chunk") {
  BUFFER_CREATE(buf, int64_t);
  REQUIRE(BUFFER_SET_CHUNK_SIZE(buf, 100) == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 100);
  status_t s;
  for (int i = 0; i < 100; ++i) BUFFER_SET(s, buf, h, i, i);
  int running;
  BUFFER_ADJUST(running, buf);
  REQUIRE(!running);
  int ok = 1;
  for (int i = 0; i < 100; ++i)
    ok = ok && BUFFER_GET(buf, h, i, -1) == i;
  REQUIRE(ok);
}

TEST("buffer adjust by chunks") {
  BUFFER_CREATE(buf, int64_t);
  REQUIRE(BUFFER_SET_CHUNK_SIZE(buf, 100) == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 100);
  status_t s;
  for (int i = 0; i < 100; ++i) BUFFER_SET(s, buf, h, i, i);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  int ok = 1;
  for (int i = 0; i < 100; ++i)
    ok = ok && BUFFER_GET(buf, h, i, -1) == i;
  REQUIRE(ok);
}

TEST("buffer adjust by chunks twice") {
  BUFFER_CREATE(buf, int64_t);
  REQUIRE(BUFFER_SET_CHUNK_SIZE(buf, 100) == STATUS_OK);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 100);
  status_t s;
  for (int i = 0; i < 100; ++i) BUFFER_SET(s, buf, h, i, i);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  BUFFER_ADJUST_DONE(buf);
  for (int i = 0; i < 100; ++i) BUFFER_ADD(s, buf, h, i, i);
  for (int _ = 1; _;) BUFFER_ADJUST(_, buf);
  int ok = 1;
  for (int i = 0; i < 100; ++i)
    ok = ok && BUFFER_GET(buf, h, i, -1) == i * 2;
  REQUIRE(ok);
}

TEST("buffer allocate two blocks") {
  BUFFER_CREATE(buf, int64_t);
  handle_t foo, bar;
  BUFFER_ALLOCATE(foo, buf, 10);
  BUFFER_ALLOCATE(bar, buf, 10);
  REQUIRE(foo.id != ID_UNDEFINED);
  REQUIRE(bar.id != ID_UNDEFINED);
  REQUIRE(foo.id != bar.id);
}

TEST("buffer fill two blocks") {
  BUFFER_CREATE(buf, int64_t);
  handle_t foo, bar;
  BUFFER_ALLOCATE(foo, buf, 10);
  BUFFER_ALLOCATE(bar, buf, 10);
  status_t s;
  BUFFER_SET(s, buf, foo, 0, 42);
  REQUIRE(s == STATUS_OK);
  BUFFER_SET(s, buf, bar, 0, 42);
  REQUIRE(s == STATUS_OK);
  BUFFER_ADJUST_LOOP(buf);
  REQUIRE(BUFFER_GET(buf, foo, 0, -1) == 42);
  REQUIRE(BUFFER_GET(buf, bar, 0, -1) == 42);
}

TEST("buffer size with 2 blocks") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 20);
  BUFFER_ALLOCATE(h, buf, 22);
  REQUIRE(buf.data.size == 42);
}

TEST("buffer size after deallocate one") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) == STATUS_OK);
  REQUIRE(buf.data.size == 10);
}

TEST("buffer size after deallocate and allocate into one") {
  BUFFER_CREATE(buf, int64_t);
  handle_t h;
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(BUFFER_DEALLOCATE(buf, h) == STATUS_OK);
  BUFFER_ALLOCATE(h, buf, 10);
  REQUIRE(buf.data.size == 10);
}

TEST("buffer deallocate efficiency") {
  BUFFER_CREATE(buf, int64_t);
  handle_t a, b;
  BUFFER_ALLOCATE(a, buf, 10);
  BUFFER_ALLOCATE(b, buf, 10);
  REQUIRE(BUFFER_DEALLOCATE(buf, b) == STATUS_OK);
  BUFFER_ALLOCATE(b, buf, 10);
  REQUIRE(buf.data.size == 20);
}

#ifdef __unix__
typedef BUFFER_TYPE(int64_t) test_buffer_int_t;

typedef struct {
  test_buffer_int_t buf;
  handle_t          h;
} test_int_buffer_data_t;

static void *test_int_add_delta(void *p) {
  test_int_buffer_data_t *data = (test_int_buffer_data_t *) p;
  status_t                s;
  for (int j = 0; j < 100; j++) {
    BUFFER_ADD(s, data->buf, data->h, 0, 1);
    BUFFER_ADD(s, data->buf, data->h, 0, -1);
  }
  BUFFER_ADD(s, data->buf, data->h, 0, 1);
  return (void *) 0;
}

TEST("buffer add delta concurrency") {
  enum { THREAD_COUNT = 100 };
  test_int_buffer_data_t data;
  BUFFER_INIT(data.buf, kit_alloc_default());
  BUFFER_ALLOCATE(data.h, data.buf, 1);
  pthread_t pool[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
    pthread_create(pool + i, NULL, test_int_add_delta, &data);
  for (int i = 0; i < THREAD_COUNT; i++) pthread_join(pool[i], NULL);
  BUFFER_ADJUST_LOOP(data.buf);
  REQUIRE(BUFFER_GET(data.buf, data.h, 0, -1) == THREAD_COUNT);
}

TEST("buffer add delta concurrency harder") {
  enum { THREAD_COUNT = 1000 };
  test_int_buffer_data_t data;
  BUFFER_INIT(data.buf, kit_alloc_default());
  BUFFER_ALLOCATE(data.h, data.buf, 1);
  pthread_t pool[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
    pthread_create(pool + i, NULL, test_int_add_delta, &data);
  for (int i = 0; i < THREAD_COUNT; i++) pthread_join(pool[i], NULL);
  BUFFER_ADJUST_LOOP(data.buf);
  REQUIRE(BUFFER_GET(data.buf, data.h, 0, -1) == THREAD_COUNT);
}

static void *test_int_adjust(void *p) {
  test_int_buffer_data_t *data = (test_int_buffer_data_t *) p;
  int                     _;
  BUFFER_ADJUST(_, data->buf);
  return (void *) 0;
}

TEST("buffer adjust concurrency") {
  enum { THREAD_COUNT = 1000, DATA_SIZE = 10000 };
  test_int_buffer_data_t data;
  BUFFER_INIT(data.buf, kit_alloc_default());
  BUFFER_SET_CHUNK_SIZE(data.buf, 10);
  BUFFER_ALLOCATE(data.h, data.buf, DATA_SIZE);
  status_t s;
  for (int i = 0; i < DATA_SIZE; i++)
    BUFFER_SET(s, data.buf, data.h, i, i);
  pthread_t pool[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
    pthread_create(pool + i, NULL, test_int_adjust, &data);
  for (int i = 0; i < THREAD_COUNT; i++) pthread_join(pool[i], NULL);
  int ok = 1;
  for (int i = 0; i < DATA_SIZE; i++)
    ok = ok && BUFFER_GET(data.buf, data.h, i, -1) == i;
  REQUIRE(ok);
}

TEST("int buffer adjust concurrency harder") {
  enum { THREAD_COUNT = 10000, DATA_SIZE = 10000 };
  test_int_buffer_data_t data;
  BUFFER_INIT(data.buf, kit_alloc_default());
  BUFFER_SET_CHUNK_SIZE(data.buf, 1);
  BUFFER_ALLOCATE(data.h, data.buf, DATA_SIZE);
  status_t s;
  for (int i = 0; i < DATA_SIZE; i++)
    BUFFER_SET(s, data.buf, data.h, i, i);
  pthread_t pool[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
    pthread_create(pool + i, NULL, test_int_adjust, &data);
  for (int i = 0; i < THREAD_COUNT; i++) pthread_join(pool[i], NULL);
  int ok = 1;
  for (int i = 0; i < DATA_SIZE; i++)
    ok = ok && BUFFER_GET(data.buf, data.h, i, -1) == i;
  REQUIRE(ok);
}

typedef BUFFER_TYPE(int8_t) test_buffer_byte_t;

typedef struct {
  test_buffer_byte_t buf;
  handle_t           h;
} test_byte_buffer_data_t;

static void *test_byte_adjust(void *p) {
  test_byte_buffer_data_t *data = (test_byte_buffer_data_t *) p;
  int                      _;
  BUFFER_ADJUST(_, data->buf);
  return (void *) 0;
}

TEST("byte buffer adjust concurrency harder") {
  enum { THREAD_COUNT = 10000, DATA_SIZE = 10000 };
  test_byte_buffer_data_t data;
  BUFFER_INIT(data.buf, kit_alloc_default());
  BUFFER_SET_CHUNK_SIZE(data.buf, 1);
  BUFFER_ALLOCATE(data.h, data.buf, DATA_SIZE);
  status_t s;
  for (int i = 0; i < DATA_SIZE; i++)
    BUFFER_SET(s, data.buf, data.h, i, (int8_t) (i % 128));
  pthread_t pool[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
    pthread_create(pool + i, NULL, test_byte_adjust, &data);
  for (int i = 0; i < THREAD_COUNT; i++) pthread_join(pool[i], NULL);
  int ok = 1;
  for (int i = 0; i < DATA_SIZE; i++)
    ok = ok &&
         BUFFER_GET(data.buf, data.h, i, -1) == (int8_t) (i % 128);
  REQUIRE(ok);
}
#endif
