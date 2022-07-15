#ifndef LAPLACE_IMPACT_H
#define LAPLACE_IMPACT_H

#include "action.h"
#include <array>
#include <variant>
#include <vector>

namespace laplace {
  struct queue_action {
    action value;

    [[nodiscard]] auto operator==(
        queue_action const &other) const noexcept -> bool {
      return this == &other;
    }
  };

  enum class mode { async, sync, control };

  struct impact {
    using value_type = std::variant<
        noop, integer_set, integer_add, byte_set, byte_add,
        integer_reserve, integer_allocate_into, integer_allocate,
        integer_deallocate, byte_reserve, byte_allocate_into,
        byte_allocate, byte_deallocate, integer_seed, integer_random,
        queue_action, tick_continue>;

    value_type value;

    impact(noop const &i) noexcept : value(i) { }
    impact(integer_set const &i) noexcept : value(i) { }
    impact(integer_add const &i) noexcept : value(i) { }
    impact(byte_set const &i) noexcept : value(i) { }
    impact(byte_add const &i) noexcept : value(i) { }
    impact(integer_reserve const &i) noexcept : value(i) { }
    impact(integer_allocate_into const &i) noexcept : value(i) { }
    impact(integer_allocate const &i) noexcept : value(i) { }
    impact(integer_deallocate const &i) noexcept : value(i) { }
    impact(byte_reserve const &i) noexcept : value(i) { }
    impact(byte_allocate_into const &i) noexcept : value(i) { }
    impact(byte_allocate const &i) noexcept : value(i) { }
    impact(byte_deallocate const &i) noexcept : value(i) { }
    impact(integer_seed const &i) noexcept : value(i) { }
    impact(integer_random const &i) noexcept : value(i) { }
    impact(queue_action const &i) noexcept : value(i) { }
    impact(tick_continue const &i) noexcept : value(i) { }

    [[nodiscard]] auto operator==(impact const &) const noexcept
        -> bool = default;
  };

  class impact_list {
  public:
    impact_list(std::pmr::memory_resource *resource =
                    default_memory_resource()) noexcept;
    impact_list(impact i, std::pmr::memory_resource *resource =
                              default_memory_resource()) noexcept;

    [[nodiscard]] auto size() const noexcept -> ptrdiff_t;
    [[nodiscard]] auto operator[](ptrdiff_t index) const noexcept
        -> impact const &;

    [[nodiscard]] auto begin() const noexcept;
    [[nodiscard]] auto end() const noexcept;

    auto operator+=(impact a) noexcept -> impact_list &;
    auto operator+=(impact_list const &a) noexcept -> impact_list &;

  private:
    using data_type = std::pmr::vector<impact>;

    data_type m_data;
  };

  [[nodiscard]] auto operator+(impact a, impact b) noexcept
      -> impact_list;

  [[nodiscard]] auto operator+(impact_list a, impact b) noexcept
      -> impact_list;

  [[nodiscard]] auto operator+(impact a, impact_list b) noexcept
      -> impact_list;

  [[nodiscard]] auto operator+(impact_list        a,
                               impact_list const &b) noexcept
      -> impact_list;

  [[nodiscard]] auto mode_of(impact const &i) noexcept -> mode;
}

#include "impact.impl.h"

#endif
