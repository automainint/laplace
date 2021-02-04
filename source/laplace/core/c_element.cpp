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
  element::element() {
    m_type_id = type_id();
  }

  element::~element() { }

  auto element::get_type_id() const -> size_t {
    return m_type_id;
  }

  void element::set_type_id(size_t id) {
    m_type_id = id;
  }

  auto element::type_id() -> size_t {
    static const type type_void;
    return type_void.get_id();
  }

  std::atomic_size_t element::type::m_count = 0;

  element::type::type() {
    m_id = m_count.fetch_add(1u, std::memory_order_acq_rel);
  }

  element::type::~type() { }

  auto element::type::get_id() const -> size_t {
    return m_id;
  }
}
