#ifndef LAPLACE_X_ENTITY_H
#define LAPLACE_X_ENTITY_H

#include "x_access.h"
#include "x_memory.h"
#include "x_operations.h"
#include <algorithm>
#include <functional>
#include <iterator>
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

    entity(std::pmr::memory_resource *resource =
               default_memory_resource()) noexcept;

    [[nodiscard]] auto error() const noexcept -> bool;

    [[nodiscard]] auto size() const noexcept -> ptrdiff_t;

    [[nodiscard]] auto point(ptrdiff_t value_id) const noexcept
        -> value_point;

    [[nodiscard]] auto index_of(ptrdiff_t id) const noexcept
        -> ptrdiff_t;

    [[nodiscard]] auto id() const noexcept -> ptrdiff_t;

    [[nodiscard]] auto setup(std::vector<field> fields) const noexcept
        -> entity;

    [[nodiscard]] auto set_id(ptrdiff_t id) const noexcept -> entity;

    [[nodiscard]] auto access() const noexcept
        -> laplace::access const &;

    [[nodiscard]] auto set_access(laplace::access a) const noexcept
        -> entity;

    [[nodiscard]] auto get(ptrdiff_t value_id,
                           int_type  def) const noexcept -> int_type;

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

    bool            m_is_error = false;
    ptrdiff_t       m_id       = id_undefined;
    fields_type     m_fields;
    laplace::access m_access;
  };
}

#include "x_entity.impl.h"

#endif
