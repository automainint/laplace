/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_BUFFER_IMPL_H
#define LAPLACE_BUFFER_IMPL_H

#include "buffer.h"

#include <limits>
#include <stdexcept>

namespace laplace {
  using std::bad_alloc, std::numeric_limits;

  template <typename int_>
  ptrdiff_t const basic_buffer<int_>::default_chunk_size =
      8000 / sizeof(int_);

  template <typename int_>
  auto basic_buffer<int_>::is_error() const noexcept -> bool {
    return m_is_error;
  }

  template <typename int_>
  auto basic_buffer<int_>::set_chunk_size(
      ptrdiff_t size) const noexcept -> basic_buffer<int_> {
    if (is_error())
      return *this;
    if (size <= 0)
      return _error();

    auto result         = *this;
    result.m_chunk_size = size;
    return result;
  }

  template <typename int_>
  auto basic_buffer<int_>::get_chunk_size() const noexcept
      -> ptrdiff_t {
    return m_chunk_size;
  }

  template <typename int_>
  auto basic_buffer<int_>::get_size() const noexcept -> ptrdiff_t {
    return m_values.size();
  }

  template <typename int_>
  auto basic_buffer<int_>::allocate(ptrdiff_t size) noexcept
      -> ptrdiff_t {
    if (is_error())
      return -1;
    if (size <= 0)
      return -1;

    auto offset = 0;

    for (; offset < m_values.size();
         offset += m_values[offset].offset) {
      if (m_values[offset].empty && m_values[offset].offset >= size)
        break;
    }

    if (size >= numeric_limits<ptrdiff_t>::max() - offset)
      return -1;

    if (offset >= m_values.size()) {
      try {
        m_values.resize(offset + size);
      } catch (bad_alloc &) { return -1; }
    }

    for (auto i = 0; i < size; ++i) {
      m_values[offset + i].empty  = false;
      m_values[offset + i].offset = size - i;
    }

    auto block = static_cast<ptrdiff_t>(m_blocks.size());
    m_blocks.push_back(offset);
    return block;
  }

  template <typename int_>
  auto basic_buffer<int_>::deallocate(ptrdiff_t block) noexcept
      -> bool {
    if (is_error())
      return false;
    if (block < 0 || block >= m_blocks.size())
      return false;
    auto begin = m_blocks[block];
    auto end   = begin;
    while (end < m_values.size()) {
      end += m_values[end].offset;
      if (end < m_values.size() && !m_values[end].empty)
        break;
    }
    for (auto i = begin; i < end; ++i) {
      m_values[i].empty  = true;
      m_values[i].offset = end - i;
    }
    m_blocks.erase(m_blocks.begin() + block);
    return true;
  }

  template <typename int_>
  auto basic_buffer<int_>::get(ptrdiff_t block, ptrdiff_t index,
                               int_ fail) const noexcept -> int_ {
    if (is_error())
      return fail;
    if (block < 0 || block >= m_blocks.size())
      return fail;
    auto const offset = m_blocks[block] + index;
    if (offset < 0 || offset >= m_values.size())
      return fail;
    return m_values[offset].value;
  }

  template <typename int_>
  auto basic_buffer<int_>::set(ptrdiff_t block, ptrdiff_t index,
                               int_ value) noexcept -> bool {
    if (is_error())
      return false;
    if (block < 0 || block >= m_blocks.size())
      return false;
    auto const offset = m_blocks[block] + index;
    if (offset < 0 || offset >= m_values.size())
      return false;
    m_values[offset].delta += value - m_values[offset].value;
    return true;
  }

  template <typename int_>
  auto basic_buffer<int_>::add_delta(ptrdiff_t block, ptrdiff_t index,
                                     int_ delta) noexcept -> bool {
    if (is_error())
      return false;
    if (block < 0 || block >= m_blocks.size())
      return false;
    auto const offset = m_blocks[block] + index;
    if (offset < 0 || offset >= m_values.size())
      return false;
    m_values[offset].delta += delta;
    return true;
  }

  template <typename int_>
  void basic_buffer<int_>::adjust() noexcept {
    if (is_error())
      return;

    for (auto &v : m_values) {
      v.value += v.delta;
      v.delta = 0;
    }
  }

  template <typename int_>
  auto basic_buffer<int_>::adjust_chunk() noexcept -> bool {
    if (is_error())
      return false;

    const auto begin   = m_next_chunk;
    const auto end     = m_next_chunk + m_chunk_size;
    const bool is_done = end >= m_values.size();

    m_next_chunk = is_done ? 0 : end;

    for (auto i = begin; i < end; ++i) {
      m_values[i].value += m_values[i].delta;
      m_values[i].delta = 0;
    }

    return !is_done;
  }

  template <typename int_>
  auto basic_buffer<int_>::_error() const noexcept
      -> basic_buffer<int_> {
    auto result       = basic_buffer<int_> {};
    result.m_is_error = true;
    return result;
  }
}

#endif
