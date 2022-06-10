/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_ENTITY_IMPL_H
#define LAPLACE_ENTITY_IMPL_H

#include <algorithm>

namespace laplace {
  constexpr auto entity::is_error() const noexcept -> bool {
    return m_is_error;
  }

  constexpr auto entity::get_size() const noexcept -> ptrdiff_t {
    return m_fields.size();
  }

  constexpr auto entity::get_id() const noexcept -> ptrdiff_t {
    return m_id;
  }

  constexpr auto entity::index_of(ptrdiff_t id) const noexcept
      -> ptrdiff_t {
    auto const i = std::lower_bound(
        m_fields.begin(), m_fields.end(), id,
        [](field const &a, ptrdiff_t b) { return a.id < b; });
    return i != m_fields.end() && i->id == id ? i - m_fields.begin()
                                              : index_undefined;
  }

  constexpr auto entity::setup(
      std::vector<field> fields) const noexcept -> entity {
    if (is_error())
      return *this;

    auto e = entity {};

    auto compare = [](field const &a, field const &b) {
      return a.id < b.id;
    };

    std::sort(fields.begin(), fields.end(), compare);

    for (ptrdiff_t i = 1; i < fields.size(); i++)
      if (fields[i].id == fields[i - 1].id)
        return _error();

    e.m_id = m_id;

    std::merge(m_fields.begin(), m_fields.end(), fields.begin(),
               fields.end(), std::back_inserter(e.m_fields), compare);

    for (ptrdiff_t i = 1; i < e.m_fields.size();) {
      if (e.m_fields[i].id == e.m_fields[i - 1].id)
        e.m_fields.erase(e.m_fields.begin() + i);
      else
        i++;
    }

    return e;
  }

  constexpr auto entity::set_id(ptrdiff_t id) const noexcept
      -> entity {
    if (is_error())
      return *this;

    auto e = entity { *this };
    e.m_id = id;
    return e;
  }

  constexpr auto entity::_error() noexcept -> entity {
    auto e       = entity {};
    e.m_is_error = true;
    return e;
  }
}

#endif
