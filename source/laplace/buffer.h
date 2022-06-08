/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_BUFFER_H
#define LAPLACE_BUFFER_H

#include "options.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace laplace {
  template <typename int_>
  class basic_buffer {
  public:
    static ptrdiff_t const default_chunk_size;

    [[nodiscard]] auto is_error() const noexcept -> bool;

    [[nodiscard]] auto set_chunk_size(ptrdiff_t size) const noexcept
        -> basic_buffer<int_>;
    [[nodiscard]] auto get_chunk_size() const noexcept -> ptrdiff_t;

    [[nodiscard]] auto get_size() const noexcept -> ptrdiff_t;

    [[nodiscard]] auto allocate(ptrdiff_t size) noexcept -> ptrdiff_t;
    [[nodiscard]] auto deallocate(ptrdiff_t block) noexcept -> bool;

    [[nodiscard]] auto get(ptrdiff_t block, ptrdiff_t index,
                           int_ fail) const noexcept -> int_;
    [[nodiscard]] auto set(ptrdiff_t block, ptrdiff_t index,
                           int_ value) noexcept -> bool;
    [[nodiscard]] auto add_delta(ptrdiff_t block, ptrdiff_t index,
                                 int_ delta) noexcept -> bool;

    void               adjust() noexcept;
    [[nodiscard]] auto adjust_chunk() noexcept -> bool;

  private:
    [[nodiscard]] auto _error() const noexcept -> basic_buffer<int_>;

    struct row {
      bool      empty  = true;
      ptrdiff_t offset = 0;
      int_      value  = 0;
      int_      delta  = 0;
    };

    bool                   m_is_error   = false;
    ptrdiff_t              m_chunk_size = default_chunk_size;
    ptrdiff_t              m_next_chunk = 0;
    std::vector<ptrdiff_t> m_blocks;
    std::vector<row>       m_values;
  };

  using buffer      = basic_buffer<value_type>;
  using byte_buffer = basic_buffer<int8_t>;
}

#endif
