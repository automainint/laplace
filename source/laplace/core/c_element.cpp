/*  laplace/core/c_element.cpp
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "element.h"

namespace laplace::core {
  using std::atomic_size_t, std::memory_order_acq_rel;

  auto element::get_type_id() const noexcept -> size_t {
    return m_type_id;
  }

  void element::set_type_id(size_t id) noexcept {
    m_type_id = id;
  }

  auto element::type_id() noexcept -> size_t {
    static const type type_void;
    return type_void.get_id();
  }

  atomic_size_t element::type::m_count = 0;

  element::type::type() noexcept {
    m_id = m_count.fetch_add(1u, memory_order_acq_rel);
  }

  auto element::type::get_id() const noexcept -> size_t {
    return m_id;
  }
}
