/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_ENTITY_H
#define LAPLACE_ENTITY_H

#include "options.h"
#include <coro/generator.h>
#include <functional>
#include <vector>

namespace laplace {
  class entity {
  public:
    struct field {
      ptrdiff_t  id    = id_undefined;
      value_type value = 0;
    };

    [[nodiscard]] constexpr auto is_error() const noexcept -> bool;

    [[nodiscard]] constexpr auto get_size() const noexcept
        -> ptrdiff_t;

    [[nodiscard]] constexpr auto get_id() const noexcept -> ptrdiff_t;

    [[nodiscard]] constexpr auto index_of(ptrdiff_t id) const noexcept
        -> ptrdiff_t;

    [[nodiscard]] constexpr auto setup(
        std::vector<field> fields) const noexcept -> entity;

    [[nodiscard]] constexpr auto set_id(ptrdiff_t id) const noexcept
        -> entity;

  private:
    [[nodiscard]] static constexpr auto _error() noexcept -> entity;

    bool               m_is_error = false;
    ptrdiff_t          m_id       = id_undefined;
    std::vector<field> m_fields;
  };
}

#include "entity.impl.h"

#endif
