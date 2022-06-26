/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_ENTITY_H
#define LAPLACE_ENTITY_H

#include "access.h"
#include "options.h"
#include <vector>

namespace laplace {
  class entity {
  public:
    struct field {
      ptrdiff_t id    = id_undefined;
      int_type  value = 0;
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

    struct spawn_options {
      ptrdiff_t return_id;
      ptrdiff_t return_index;
    };

    [[nodiscard]] auto spawn(spawn_options options) const noexcept
        -> impact;

    [[nodiscard]] auto spawn() const noexcept -> impact;

    [[nodiscard]] auto remove() const noexcept -> impact;

    [[nodiscard]] auto set(ptrdiff_t value_id,
                           int_type  value) const noexcept -> impact;

    [[nodiscard]] auto set_by_index(ptrdiff_t index,
                                    int_type  value) const noexcept
        -> impact;

    [[nodiscard]] auto get(ptrdiff_t value_id,
                           int_type  def) const noexcept -> int_type;

    [[nodiscard]] auto get_by_index(ptrdiff_t index,
                                    int_type  def) const noexcept
        -> int_type;

  private:
    [[nodiscard]] static auto _error() noexcept -> entity;

    bool               m_is_error = false;
    ptrdiff_t          m_id       = id_undefined;
    access             m_access;
    std::vector<field> m_fields;
  };
}

#include "entity.impl.h"

#endif
