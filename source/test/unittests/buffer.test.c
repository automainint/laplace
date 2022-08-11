#include "../../laplace/buffer.h"

#define KIT_TEST_FILE buffer
#include <kit_test/test.h>

TEST("create int buffer") {
  BUFFER_CREATE(buf, int64_t);
  REQUIRE(!BUFFER_ERROR(buf));
}

TEST("create byte buffer") {
  BUFFER_CREATE(buf, int8_t);
  REQUIRE(!BUFFER_ERROR(buf));
}

TEST("buffer get chunk size") {
  BUFFER_CREATE(buf, int64_t);
  REQUIRE(BUFFER_CHUNK_SIZE(buf) ==
          BUFFER_DEFAULT_CHUNK_SIZE / sizeof(int64_t));
}

TEST("buffer set chunk size") {
  BUFFER_CREATE(buf, int64_t);
  BUFFER_SET_CHUNK_SIZE(buf, 42);
  REQUIRE(BUFFER_CHUNK_SIZE(buf) == 42);
}

TEST("buffer set chunk size may fail") {
  BUFFER_CREATE(buf, int64_t);
  BUFFER_SET_CHUNK_SIZE(buf, -1);
  REQUIRE(BUFFER_ERROR(buf) == BUFFER_ERROR_INVALID_CHUNK_SIZE);
}

TEST("buffer propagate error") {
  BUFFER_CREATE(buf, int64_t);
  BUFFER_SET_CHUNK_SIZE(buf, -1);
  BUFFER_SET_CHUNK_SIZE(buf, 42);
  REQUIRE(BUFFER_ERROR(buf) == BUFFER_ERROR_INVALID_CHUNK_SIZE);
}

TEST("buffer allocate") {
  BUFFER_CREATE(buf, int64_t);
  buffer_handle_t h = BUFFER_ALLOCATE(buf, 10);
  REQUIRE(h.id != ID_UNDEFINED);
}

TEST("buffer allocate negative size will fail") {
  BUFFER_CREATE(buf, int64_t);
  buffer_handle_t h = BUFFER_ALLOCATE(buf, -1);
  REQUIRE(h.id == ID_UNDEFINED);
}

TEST("buffer allocate zero size will fail") {
  BUFFER_CREATE(buf, int64_t);
  buffer_handle_t h = BUFFER_ALLOCATE(buf, 0);
  REQUIRE(h.id == ID_UNDEFINED);
}

TEST("buffer allocate too big will fail") {
  BUFFER_CREATE(buf, int64_t);
  buffer_handle_t h = BUFFER_ALLOCATE(buf, PTRDIFF_MAX);
  REQUIRE(h.id == ID_UNDEFINED);
}

TEST("buffer allocate after error will fail") {
  BUFFER_CREATE(buf, int64_t);
  BUFFER_SET_CHUNK_SIZE(buf, -1);
  buffer_handle_t h = BUFFER_ALLOCATE(buf, 10);
  REQUIRE(h.id == ID_UNDEFINED);
}

TEST("buffer allocate into") {
  BUFFER_CREATE(buf, int64_t);
  buffer_handle_t h = { .id = 42, .generation = -1 };
  h                 = BUFFER_ALLOCATE_INTO(buf, 10, h);
  REQUIRE(h.id != ID_UNDEFINED);
  REQUIRE(h.generation == 0);
}

TEST("buffer allocate into undefined will fail") {
  BUFFER_CREATE(buf, int64_t);
  buffer_handle_t h = { .id = ID_UNDEFINED, .generation = -1 };
  h                 = BUFFER_ALLOCATE_INTO(buf, 10, h);
  REQUIRE(h.id == ID_UNDEFINED);
  REQUIRE(h.error == LAPLACE_BUFFER_ERROR_INVALID_HANDLE_ID);
}

TEST("buffer allocate into wrong generation will fail") {
  BUFFER_CREATE(buf, int64_t);
  buffer_handle_t h = { .id = 42, .generation = 0 };
  h                 = BUFFER_ALLOCATE_INTO(buf, 10, h);
  REQUIRE(h.id == ID_UNDEFINED);
  REQUIRE(h.error == LAPLACE_BUFFER_ERROR_INVALID_HANDLE_GENERATION);
}

TEST("buffer allocate into invalid size will fail") {
  BUFFER_CREATE(buf, int64_t);
  buffer_handle_t h = { .id = 42, .generation = -1 };
  h                 = BUFFER_ALLOCATE_INTO(buf, -1, h);
  REQUIRE(h.id == ID_UNDEFINED);
  REQUIRE(h.error == LAPLACE_BUFFER_ERROR_INVALID_SIZE);
}

TEST("buffer allocate into propagate error") {
  BUFFER_CREATE(buf, int64_t);
  buffer_handle_t h = { .id = 42, .generation = -1 };
  BUFFER_SET_CHUNK_SIZE(buf, -1);
  h = BUFFER_ALLOCATE_INTO(buf, 10, h);
  REQUIRE(h.id == ID_UNDEFINED);
  REQUIRE(h.error == LAPLACE_BUFFER_ERROR_INVALID_BUFFER);
}

TEST("buffer allocate and allocate into") {
  BUFFER_CREATE(buf, int64_t);
  buffer_handle_t h = BUFFER_ALLOCATE(buf, 10);
  h                 = BUFFER_ALLOCATE_INTO(buf, 100, h);
  REQUIRE(h.id != ID_UNDEFINED);
  REQUIRE(h.generation == 1);
}
