#ifndef LAPLACE_OPTIONS_H
#define LAPLACE_OPTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef int     laplace_status_t;
typedef int64_t laplace_time_t;
typedef int64_t laplace_integer_t;
typedef int8_t  laplace_byte_t;

enum { LAPLACE_STATUS_OK = 0, LAPLACE_ID_UNDEFINED = -1 };

#ifndef LAPLACE_DISABLE_SHORT_NAMES
#  define ID_UNDEFINED LAPLACE_ID_UNDEFINED
#  define STATUS_OK LAPLACE_STATUS_OK

#  define status_t laplace_status_t
#endif

#ifdef __cplusplus
}

namespace laplace {
  using time_type = int64_t;
  using int_type  = int64_t;
  using byte_type = int8_t;

  static constexpr ptrdiff_t id_undefined    = -1;
  static constexpr ptrdiff_t index_undefined = -1;

  [[nodiscard]] auto default_thread_count() noexcept -> ptrdiff_t;

  [[nodiscard]] auto overthreading_limit() noexcept -> ptrdiff_t;

  [[nodiscard]] auto average_impact_list_size() noexcept -> ptrdiff_t;

  [[nodiscard]] auto average_async_impact_count() noexcept
      -> ptrdiff_t;

  [[nodiscard]] auto average_sync_impact_count() noexcept
      -> ptrdiff_t;

  [[nodiscard]] auto average_fork_count() noexcept -> ptrdiff_t;
}
#endif

#endif
