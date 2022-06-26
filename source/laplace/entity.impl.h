/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_ENTITY_IMPL_H
#define LAPLACE_ENTITY_IMPL_H

#include <algorithm>
#include <iterator>

namespace laplace {
  inline auto entity::is_error() const noexcept -> bool {
    return m_is_error;
  }

  inline auto entity::get_size() const noexcept -> ptrdiff_t {
    return m_fields.size();
  }

  inline auto entity::get_id() const noexcept -> ptrdiff_t {
    return m_id;
  }

  inline auto entity::get_access() const noexcept -> access const & {
    return m_access;
  }

  inline auto entity::index_of(ptrdiff_t id) const noexcept
      -> ptrdiff_t {
    auto const i = std::lower_bound(
        m_fields.begin(), m_fields.end(), id,
        [](field const &a, ptrdiff_t b) { return a.id < b; });
    return i != m_fields.end() && i->id == id ? i - m_fields.begin()
                                              : index_undefined;
  }

  inline auto entity::setup(std::vector<field> fields) const noexcept
      -> entity {
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

  inline auto entity::set_id(ptrdiff_t id) const noexcept -> entity {
    if (is_error())
      return *this;

    auto e = entity { *this };
    e.m_id = id;
    return e;
  }

  inline auto entity::set_access(access a) const noexcept -> entity {
    if (is_error())
      return *this;

    auto e     = entity { *this };
    e.m_access = std::move(a);
    return e;
  }

  inline auto entity::spawn() const noexcept -> impact {
    if (is_error())
      return noop {};

    return integer_reallocate { .id = get_id(), .size = get_size() };
  }

  inline auto entity::remove() const noexcept -> impact {
    if (is_error())
      return noop {};

    return integer_deallocate { .id = get_id() };
  }

  inline auto entity::get(ptrdiff_t value_id,
                          int_type  def) const noexcept -> int_type {
    if (is_error())
      return def;
    return m_access.get_integer(get_id(), index_of(value_id), def);
  }

  inline auto entity::set(ptrdiff_t value_id,
                          int_type  value) const noexcept -> impact {
    if (is_error())
      return noop {};

    return integer_set { .id    = get_id(),
                         .index = index_of(value_id),
                         .value = value };
  }

  inline auto entity::add(ptrdiff_t value_id,
                          int_type  delta) const noexcept -> impact {
    if (is_error())
      return noop {};

    return integer_add { .id    = get_id(),
                         .index = index_of(value_id),
                         .delta = delta };
  }

  inline auto entity::random(ptrdiff_t value_id, int_type min,
                             int_type max) const noexcept -> impact {
    if (is_error())
      return noop {};

    return laplace::random { .min          = min,
                             .max          = max,
                             .return_id    = get_id(),
                             .return_index = index_of(value_id),
                             .return_size  = 1 };
  }

  inline auto entity::point(ptrdiff_t value_id) const noexcept
      -> value_point {
    return { .id = get_id(), .index = index_of(value_id) };
  }

  inline auto entity::spawn_to(value_point p) const noexcept
      -> impact {
    if (is_error())
      return noop {};

    return integer_allocate { .size         = get_size(),
                              .return_id    = p.id,
                              .return_index = p.index };
  }

  inline auto entity::_error() noexcept -> entity {
    auto e       = entity {};
    e.m_is_error = true;
    return e;
  }
}

#endif
