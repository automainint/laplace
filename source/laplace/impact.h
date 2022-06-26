/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_IMPACT_H
#define LAPLACE_IMPACT_H

#include "options.h"
#include <array>
#include <cstddef>
#include <memory_resource>
#include <variant>
#include <vector>

namespace laplace {
  struct noop {
    [[nodiscard]] auto operator==(noop const &other) const noexcept
        -> bool = default;
  };

  struct tick_continue {
    [[nodiscard]] auto operator==(
        tick_continue const &other) const noexcept -> bool = default;
  };

  struct integer_reserve {
    ptrdiff_t count;

    [[nodiscard]] auto operator==(integer_reserve const &other)
        const noexcept -> bool = default;
  };

  struct integer_reallocate {
    ptrdiff_t id;
    ptrdiff_t size;

    [[nodiscard]] auto operator==(integer_reallocate const &other)
        const noexcept -> bool = default;
  };

  struct integer_allocate {
    ptrdiff_t size;
    ptrdiff_t return_id;
    ptrdiff_t return_index;

    [[nodiscard]] auto operator==(integer_allocate const &other)
        const noexcept -> bool = default;
  };

  struct integer_deallocate {
    ptrdiff_t id;

    [[nodiscard]] auto operator==(integer_deallocate const &other)
        const noexcept -> bool = default;
  };

  struct integer_set {
    ptrdiff_t id;
    ptrdiff_t index;
    int_type  value;

    [[nodiscard]] auto operator==(
        integer_set const &other) const noexcept -> bool = default;
  };

  struct integer_add {
    ptrdiff_t id;
    ptrdiff_t index;
    int_type  delta;

    [[nodiscard]] auto operator==(
        integer_add const &other) const noexcept -> bool = default;
  };

  struct byte_reserve {
    ptrdiff_t count;

    [[nodiscard]] auto operator==(
        byte_reserve const &other) const noexcept -> bool = default;
  };

  struct byte_reallocate {
    ptrdiff_t id;
    ptrdiff_t size;

    [[nodiscard]] auto operator==(byte_reallocate const &other)
        const noexcept -> bool = default;
  };

  struct byte_allocate {
    ptrdiff_t size;
    ptrdiff_t return_id;
    ptrdiff_t return_index;

    [[nodiscard]] auto operator==(
        byte_allocate const &other) const noexcept -> bool = default;
  };

  struct byte_deallocate {
    ptrdiff_t id;

    [[nodiscard]] auto operator==(byte_deallocate const &other)
        const noexcept -> bool = default;
  };

  struct byte_set {
    ptrdiff_t id;
    ptrdiff_t index;
    byte_type value;

    [[nodiscard]] auto operator==(
        byte_set const &other) const noexcept -> bool = default;
  };

  struct byte_add {
    ptrdiff_t id;
    ptrdiff_t index;
    byte_type delta;

    [[nodiscard]] auto operator==(
        byte_add const &other) const noexcept -> bool = default;
  };

  struct random {
    int_type  min;
    int_type  max;
    ptrdiff_t return_id;
    ptrdiff_t return_index;
    ptrdiff_t return_size;

    [[nodiscard]] auto operator==(random const &other) const noexcept
        -> bool = default;
  };

  using impact = std::variant<
      noop, integer_set, integer_add, byte_set, byte_add,
      integer_reserve, integer_reallocate, integer_allocate,
      integer_deallocate, byte_reserve, byte_reallocate,
      byte_allocate, byte_deallocate, random, tick_continue>;

  struct impact_list {
  public:
    impact_list() noexcept = default;
    explicit impact_list(impact i) noexcept;

    [[nodiscard]] auto get_size() const noexcept -> ptrdiff_t;
    [[nodiscard]] auto add(impact i) const noexcept -> impact_list;
    [[nodiscard]] auto add(impact_list i) const noexcept
        -> impact_list;

    [[nodiscard]] auto operator[](ptrdiff_t index) const noexcept
        -> impact const &;

  private:
    std::pmr::vector<impact> m_data;
  };

  [[nodiscard]] auto operator+(impact a, impact b) noexcept
      -> impact_list;
  [[nodiscard]] auto operator+(impact_list const &a,
                               impact b) noexcept -> impact_list;
  [[nodiscard]] auto operator+(impact             a,
                               impact_list const &b) noexcept
      -> impact_list;
  [[nodiscard]] auto operator+(impact_list const &a,
                               impact_list const &b) noexcept
      -> impact_list;
}

#endif
