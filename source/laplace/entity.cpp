/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "entity.h"

#include "impact.h"
#include <algorithm>
#include <iterator>

namespace laplace {
  auto entity::is_error() const noexcept -> bool {
    return m_is_error;
  }

  auto entity::get_size() const noexcept -> ptrdiff_t {
    return _bind<ptrdiff_t>([&]() { return m_fields.size(); },
                            index_undefined);
  }

  auto entity::get_id() const noexcept -> ptrdiff_t {
    return _bind<ptrdiff_t>([&]() { return m_id; }, id_undefined);
  }

  auto entity::get_access() const noexcept -> access const & {
    return m_access;
  }

  auto entity::index_of(ptrdiff_t id) const noexcept -> ptrdiff_t {
    return _bind<ptrdiff_t>(
        [&]() {
          auto const i = std::lower_bound(
              m_fields.begin(), m_fields.end(), id,
              [](field const &a, ptrdiff_t b) { return a.id < b; });
          return i != m_fields.end() && i->id == id
                     ? i - m_fields.begin()
                     : index_undefined;
        },
        index_undefined);
  }

  auto entity::setup(std::vector<field> fields) const noexcept
      -> entity {
    return _bind([&]() {
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
                 fields.end(), std::back_inserter(e.m_fields),
                 compare);

      for (ptrdiff_t i = 1; i < e.m_fields.size();) {
        if (e.m_fields[i].id == e.m_fields[i - 1].id)
          e.m_fields.erase(e.m_fields.begin() + i);
        else
          i++;
      }

      return e;
    });
  }

  auto entity::set_id(ptrdiff_t id) const noexcept -> entity {
    return _bind([&]() {
      auto e = entity { *this };
      e.m_id = id;
      return e;
    });
  }

  auto entity::set_access(access a) const noexcept -> entity {
    return _bind([&]() {
      auto e     = entity { *this };
      e.m_access = std::move(a);
      return e;
    });
  }

  auto entity::get(ptrdiff_t value_id, int_type def) const noexcept
      -> int_type {
    return _bind<int_type>(
        [&]() {
          return m_access.get_integer(get_id(), index_of(value_id),
                                      def);
        },
        def);
  }

  auto entity::point(ptrdiff_t value_id) const noexcept
      -> value_point {
    return { .id = get_id(), .index = index_of(value_id) };
  }

  auto entity::spawn() const noexcept -> impact {
    return _bind<impact>(
        [&]() {
          return integer_allocate_into { .id   = get_id(),
                                         .size = get_size() };
        },
        noop {});
  }

  auto entity::remove() const noexcept -> impact {
    return _bind<impact>(
        [&]() { return integer_deallocate { .id = get_id() }; },
        noop {});
  }

  auto entity::set(ptrdiff_t value_id, int_type value) const noexcept
      -> impact {
    return _bind<impact>(
        [&]() {
          return integer_set { .id    = get_id(),
                               .index = index_of(value_id),
                               .value = value };
        },
        noop {});
  }

  auto entity::add(ptrdiff_t value_id, int_type delta) const noexcept
      -> impact {
    return _bind<impact>(
        [&]() {
          return integer_add { .id    = get_id(),
                               .index = index_of(value_id),
                               .delta = delta };
        },
        noop {});
  }

  auto entity::random(ptrdiff_t value_id, int_type min,
                      int_type max) const noexcept -> impact {
    return _bind<impact>(
        [&]() {
          return integer_random { .min          = min,
                                  .max          = max,
                                  .return_id    = get_id(),
                                  .return_index = index_of(value_id),
                                  .return_size  = 1 };
        },
        noop {});
  }

  auto entity::spawn_to(value_point p) const noexcept -> impact {
    return _bind<impact>(
        [&]() {
          return integer_allocate { .size         = get_size(),
                                    .return_id    = p.id,
                                    .return_index = p.index };
        },
        noop {});
  }

  auto entity::_error() const noexcept -> entity {
    auto e       = entity { *this };
    e.m_is_error = true;
    return e;
  }

  auto entity::_bind(std::function<entity()> f) const noexcept
      -> entity {
    if (is_error())
      return *this;
    return f();
  }

  template <typename type_>
  auto entity::_bind(std::function<type_()> f,
                     type_ def) const noexcept -> type_ {
    if (is_error())
      return def;
    return f();
  }
}
