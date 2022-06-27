/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "entity.h"

#include "impact.h"

namespace laplace {
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
}
