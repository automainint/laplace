/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_ENTITY_H
#define LAPLACE_ENTITY_H

#include "access.h"
#include "memory.h"
#include "operations.h"
#include <functional>
#include <vector>

namespace laplace {
  class impact;

  class entity {
  public:
    struct field {
      ptrdiff_t id = id_undefined;
    };

    struct value_point {
      ptrdiff_t id;
      ptrdiff_t index;
    };

    [[nodiscard]] auto is_error() const noexcept -> bool;

    [[nodiscard]] auto get_size() const noexcept -> ptrdiff_t;

    [[nodiscard]] auto get_id() const noexcept -> ptrdiff_t;

    [[nodiscard]] auto get_access() const noexcept -> access const &;

    [[nodiscard]] auto index_of(ptrdiff_t id) const noexcept
        -> ptrdiff_t;

    [[nodiscard]] auto setup(std::vector<field> fields) const noexcept
        -> entity;

    [[nodiscard]] auto set_id(ptrdiff_t id) const noexcept -> entity;

    [[nodiscard]] auto set_access(access a) const noexcept -> entity;

    [[nodiscard]] auto get(ptrdiff_t value_id,
                           int_type  def) const noexcept -> int_type;

    [[nodiscard]] auto point(ptrdiff_t value_id) const noexcept
        -> value_point;

    [[nodiscard]] auto spawn() const noexcept -> impact;

    [[nodiscard]] auto remove() const noexcept -> impact;

    [[nodiscard]] auto set(ptrdiff_t value_id,
                           int_type  value) const noexcept -> impact;

    [[nodiscard]] auto add(ptrdiff_t value_id,
                           int_type  delta) const noexcept -> impact;

    [[nodiscard]] auto random(ptrdiff_t value_id, int_type min,
                              int_type max) const noexcept -> impact;

    [[nodiscard]] auto spawn_to(value_point p) const noexcept
        -> impact;

  private:
    [[nodiscard]] auto _error() const noexcept -> entity;
    [[nodiscard]] auto _bind(std::function<entity()> f) const noexcept
        -> entity;
    template <typename type_>
    [[nodiscard]] auto _bind(std::function<type_()> f,
                             type_ def) const noexcept -> type_;

    using fields_type = std::pmr::vector<field>;

    bool        m_is_error = false;
    ptrdiff_t   m_id       = id_undefined;
    access      m_access;
    fields_type m_fields = fields_type { &memory_resource };
  };
}

#endif
