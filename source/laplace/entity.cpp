/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "entity.h"

#include "impact.h"
#include <algorithm>
#include <iterator>

namespace laplace {
  using std::vector;

  auto entity::error() const noexcept -> bool {
    return m_is_error;
  }

  auto entity::size() const noexcept -> ptrdiff_t {
    return _bind<ptrdiff_t>([&]() { return m_size; },
                            index_undefined);
  }

  auto entity::id() const noexcept -> ptrdiff_t {
    return _bind<ptrdiff_t>([&]() { return m_id; }, id_undefined);
  }

  auto entity::access() const noexcept -> laplace::access const & {
    return m_access;
  }

  auto entity::index_of(ptrdiff_t id) const noexcept -> ptrdiff_t {
    return _bind<ptrdiff_t>(
        [&]() {
          if (id < 0 || id >= m_indices.size())
            return index_undefined;
          return m_indices[id];
        },
        index_undefined);
  }

  auto entity::setup(vector<field> fields) const noexcept -> entity {
    return _bind([&]() {
      auto e = entity { *this };

      for (auto &f : fields) {
        auto const id = f.id;
        if (id < 0)
          return _error();
        if (e.m_indices.size() <= id)
          e.m_indices.resize(id + 1, index_undefined);
        if (e.m_indices[id] == index_undefined)
          e.m_indices[id] = e.m_size++;
        else if (id >= m_indices.size() ||
                 m_indices[id] == index_undefined)
          return _error();
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

  auto entity::set_access(laplace::access a) const noexcept
      -> entity {
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
          return m_access.get_integer(id(), index_of(value_id), def);
        },
        def);
  }

  auto entity::point(ptrdiff_t value_id) const noexcept
      -> value_point {
    return { .id = id(), .index = index_of(value_id) };
  }

  auto entity::spawn() const noexcept -> impact {
    return _bind<impact>(
        [&]() {
          return integer_allocate_into { .id = id(), .size = size() };
        },
        noop {});
  }

  auto entity::remove() const noexcept -> impact {
    return _bind<impact>(
        [&]() { return integer_deallocate { .id = id() }; }, noop {});
  }

  auto entity::set(ptrdiff_t value_id, int_type value) const noexcept
      -> impact {
    return _bind<impact>(
        [&]() {
          return integer_set { .id    = id(),
                               .index = index_of(value_id),
                               .value = value };
        },
        noop {});
  }

  auto entity::add(ptrdiff_t value_id, int_type delta) const noexcept
      -> impact {
    return _bind<impact>(
        [&]() {
          return integer_add { .id    = id(),
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
                                  .return_id    = id(),
                                  .return_index = index_of(value_id),
                                  .return_size  = 1 };
        },
        noop {});
  }

  auto entity::spawn_to(value_point p) const noexcept -> impact {
    return _bind<impact>(
        [&]() {
          return integer_allocate { .size         = size(),
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
    if (error())
      return *this;
    return f();
  }

  template <typename type_>
  auto entity::_bind(std::function<type_()> f,
                     type_ def) const noexcept -> type_ {
    if (error())
      return def;
    return f();
  }
}
