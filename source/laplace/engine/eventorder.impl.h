/*  laplace/engine/eventorder.impl.h
 *
 *      Event order implementation.
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

#ifndef __laplace__engine_eventorder_impl__
#define __laplace__engine_eventorder_impl__

namespace laplace::engine {
  constexpr eventorder::eventorder(size_t index) {
    this->m_indices[0] = index;
    this->m_size       = 1;
  }

  constexpr auto eventorder::spawn(ref_uint child_count) const
      -> eventorder {
    return eventorder(*this, child_count++);
  }

  constexpr auto eventorder::operator<(
      const eventorder &order) const -> bool {
    if (this->m_size < order.m_size)
      return true;

    if (order.m_size == 0)
      return this->m_size > 0;

    for (size_t i = 0; i < this->m_size && i < order.m_size;
         i++) {
      if (this->m_indices[i] < order.m_indices[i])
        return true;
    }

    return false;
  }

  constexpr auto eventorder::get_index() const -> size_t {
    return this->m_size > 0 ? this->m_indices[0] : -1;
  }

  constexpr eventorder::eventorder(
      const eventorder &parent, size_t index) {
    if (parent.m_size == eventorder::max_depth) {
      this->m_size = 0;
      return;
    }

    std::copy(parent.m_indices.begin(),
        parent.m_indices.begin() + parent.m_size,
        this->m_indices.begin());

    this->m_indices[parent.m_size] = index;
    this->m_size                   = parent.m_size + 1;
  }
}

#endif
