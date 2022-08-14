#include "x_entity.h"

#include "x_impact.h"

namespace laplace {
  using std::pmr::memory_resource;

  entity::entity(memory_resource *resource) noexcept :
      m_fields(resource) { }

  auto entity::access() const noexcept -> laplace::access const & {
    return m_access;
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

  auto entity::spawn() const noexcept -> impact {
    return _bind<impact>(
        [&]() {
          return impact { integer_allocate_into { .id   = id(),
                                                  .size = size() } };
        },
        impact { noop {} });
  }

  auto entity::remove() const noexcept -> impact {
    return _bind<impact>(
        [&]() {
          return impact { integer_deallocate { .id = id() } };
        },
        impact { noop {} });
  }

  auto entity::set(ptrdiff_t value_id, int_type value) const noexcept
      -> impact {
    return _bind<impact>(
        [&]() {
          return impact { integer_set { .id    = id(),
                                        .index = index_of(value_id),
                                        .value = value } };
        },
        impact { noop {} });
  }

  auto entity::add(ptrdiff_t value_id, int_type delta) const noexcept
      -> impact {
    return _bind<impact>(
        [&]() {
          return impact { integer_add { .id    = id(),
                                        .index = index_of(value_id),
                                        .delta = delta } };
        },
        impact { noop {} });
  }

  auto entity::random(ptrdiff_t value_id, int_type min,
                      int_type max) const noexcept -> impact {
    return _bind<impact>(
        [&]() {
          return impact { integer_random {
              .min          = min,
              .max          = max,
              .return_id    = id(),
              .return_index = index_of(value_id),
              .return_size  = 1 } };
        },
        impact { noop {} });
  }

  auto entity::spawn_to(value_point p) const noexcept -> impact {
    return _bind<impact>(
        [&]() {
          return impact { integer_allocate {
              .size         = size(),
              .return_id    = p.id,
              .return_index = p.index } };
        },
        impact { noop {} });
  }
}
