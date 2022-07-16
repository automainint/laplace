#ifndef LAPLACE_OPTIONS_H
#define LAPLACE_OPTIONS_H

#include <cstddef>
#include <cstdint>

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
