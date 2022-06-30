/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_BUFFER_H
#define LAPLACE_BUFFER_H

#include "options.h"
#include <atomic>
#include <concepts>
#include <functional>
#include <vector>

namespace laplace {
  template <typename int_>
  concept atomic_integer =
      requires(int_ val, int_ hlp) {
        val.store(hlp.load(std::memory_order_relaxed),
                  std::memory_order_relaxed);
        val.load(std::memory_order_relaxed);
        val.fetch_add(hlp.load(std::memory_order_relaxed),
                      std::memory_order_relaxed);
        val.exchange(hlp.load(std::memory_order_relaxed),
                     std::memory_order_relaxed);
      };

  template <typename int_,
            atomic_integer atomic_int_ = std::atomic<int_>>
  class basic_buffer {
  public:
    static ptrdiff_t const default_chunk_size;

    basic_buffer() noexcept = default;
    basic_buffer(basic_buffer const &buf) noexcept;
    basic_buffer(basic_buffer &&) noexcept = default;
    ~basic_buffer() noexcept               = default;
    auto operator=(basic_buffer const &buf) noexcept
        -> basic_buffer &;
    auto operator=(basic_buffer &&) noexcept
        -> basic_buffer & = default;

    [[nodiscard]] auto error() const noexcept -> bool;

    [[nodiscard]] auto set_chunk_size(ptrdiff_t size) const noexcept
        -> basic_buffer<int_, atomic_int_>;

    [[nodiscard]] auto chunk_size() const noexcept -> ptrdiff_t;

    [[nodiscard]] auto size() const noexcept -> ptrdiff_t;

    [[nodiscard]] auto reserve(ptrdiff_t count) noexcept -> bool;
    [[nodiscard]] auto allocate(ptrdiff_t size) noexcept -> ptrdiff_t;
    [[nodiscard]] auto allocate_into(ptrdiff_t block,
                                     ptrdiff_t size) noexcept -> bool;
    [[nodiscard]] auto deallocate(ptrdiff_t block) noexcept -> bool;

    [[nodiscard]] auto get(ptrdiff_t block, ptrdiff_t index,
                           int_ fail) const noexcept -> int_;
    [[nodiscard]] auto set(ptrdiff_t block, ptrdiff_t index,
                           int_ value) noexcept -> bool;
    [[nodiscard]] auto add(ptrdiff_t block, ptrdiff_t index,
                           int_ delta) noexcept -> bool;

    [[nodiscard]] auto adjust() noexcept -> bool;
    void               adjust_done() noexcept;

  private:
    [[nodiscard]] auto _error() const noexcept
        -> basic_buffer<int_, atomic_int_>;
    [[nodiscard]] auto _bind(
        std::function<basic_buffer<int_, atomic_int_>()> f)
        const noexcept -> basic_buffer<int_, atomic_int_>;
    template <typename type_>
    [[nodiscard]] auto _bind(std::function<type_()> f,
                             type_ def) const noexcept -> type_;

    [[nodiscard]] auto _alloc(ptrdiff_t size) noexcept -> ptrdiff_t;
    void               _free(ptrdiff_t offset) noexcept;

    struct row {
      bool        empty  = true;
      ptrdiff_t   offset = 0;
      int_        value  = 0;
      atomic_int_ delta  = 0;

      row() noexcept = default;
      row(row const &r) noexcept;
      row(row &&) noexcept = default;
      ~row() noexcept      = default;
      auto operator=(row const &r) noexcept -> row &;
      auto operator=(row &&) noexcept -> row & = default;
    };

    bool                   m_is_error   = false;
    ptrdiff_t              m_chunk_size = default_chunk_size;
    ptrdiff_t              m_reserved   = 0;
    ptrdiff_t              m_next_block = 0;
    std::atomic<ptrdiff_t> m_next_chunk = 0;
    std::vector<ptrdiff_t> m_blocks;
    std::vector<row>       m_values;
  };

  using buffer      = basic_buffer<int_type>;
  using byte_buffer = basic_buffer<byte_type>;
}

#endif
