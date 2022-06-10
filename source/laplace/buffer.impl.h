/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_BUFFER_IMPL_H
#define LAPLACE_BUFFER_IMPL_H

#include "buffer.h"

#include <limits>
#include <stdexcept>

namespace laplace {
  using std::bad_alloc, std::numeric_limits,
      std::memory_order_relaxed, std::min;

  template <typename int_, atomic_integer atomic_int_>
  basic_buffer<int_, atomic_int_>::basic_buffer(
      basic_buffer const &buf) noexcept :
      m_is_error(buf.m_is_error),
      m_chunk_size(buf.m_chunk_size),
      m_next_chunk(buf.m_next_chunk.load(memory_order_relaxed)),
      m_blocks(buf.m_blocks), m_values(buf.m_values) { }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::operator=(
      basic_buffer const &buf) noexcept -> basic_buffer & {
    m_is_error   = buf.m_is_error;
    m_chunk_size = buf.m_chunk_size;
    m_next_chunk.store(buf.m_next_chunk.load(memory_order_relaxed),
                       memory_order_relaxed);
    m_blocks = buf.m_blocks;
    m_values = buf.m_values;
    return *this;
  }

  template <typename int_, atomic_integer atomic_int_>
  ptrdiff_t const
      basic_buffer<int_, atomic_int_>::default_chunk_size =
          8000 / sizeof(int_);

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::is_error() const noexcept
      -> bool {
    return m_is_error;
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::set_chunk_size(ptrdiff_t size)
      const noexcept -> basic_buffer<int_, atomic_int_> {
    if (is_error())
      return *this;
    if (size <= 0)
      return _error();

    auto result         = *this;
    result.m_chunk_size = size;
    return result;
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::get_chunk_size()
      const noexcept -> ptrdiff_t {
    return m_chunk_size;
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::get_size() const noexcept
      -> ptrdiff_t {
    return m_values.size();
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::allocate(
      ptrdiff_t size) noexcept -> ptrdiff_t {
    if (is_error())
      return id_undefined;
    if (size <= 0)
      return id_undefined;

    auto offset = 0;

    for (; offset < m_values.size();
         offset += m_values[offset].offset) {
      if (m_values[offset].empty && m_values[offset].offset >= size)
        break;
    }

    if (size >= numeric_limits<ptrdiff_t>::max() - offset)
      return id_undefined;

    if (offset >= m_values.size()) {
      try {
        m_values.resize(offset + size);
      } catch (bad_alloc &) { return id_undefined; }
    }

    for (auto i = 0; i < size; ++i) {
      m_values[offset + i].empty  = false;
      m_values[offset + i].offset = size - i;
    }

    auto block = static_cast<ptrdiff_t>(m_blocks.size());
    m_blocks.push_back(offset);
    return block;
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::deallocate(
      ptrdiff_t block) noexcept -> bool {
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

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::get(ptrdiff_t block,
                                            ptrdiff_t index,
                                            int_ fail) const noexcept
      -> int_ {
    if (is_error())
      return fail;
    if (block < 0 || block >= m_blocks.size())
      return fail;
    auto const offset = m_blocks[block] + index;
    if (offset < 0 || offset >= m_values.size())
      return fail;
    return m_values[offset].value;
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::set(ptrdiff_t block,
                                            ptrdiff_t index,
                                            int_      value) noexcept
      -> bool {
    if (is_error())
      return false;
    if (block < 0 || block >= m_blocks.size())
      return false;
    auto const offset = m_blocks[block] + index;
    if (offset < 0 || offset >= m_values.size())
      return false;
    m_values[offset].delta.fetch_add(value - m_values[offset].value,
                                     memory_order_relaxed);
    return true;
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::add(ptrdiff_t block,
                                            ptrdiff_t index,
                                            int_      delta) noexcept
      -> bool {
    if (is_error())
      return false;
    if (block < 0 || block >= m_blocks.size())
      return false;
    auto const offset = m_blocks[block] + index;
    if (offset < 0 || offset >= m_values.size())
      return false;
    m_values[offset].delta.fetch_add(delta, memory_order_relaxed);
    return true;
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::adjust() noexcept -> bool {
    if (is_error())
      return false;

    const auto begin   = m_next_chunk.fetch_add(m_chunk_size,
                                                memory_order_relaxed);
    const auto end     = min<ptrdiff_t>(begin + m_chunk_size,
                                    m_values.size());
    const bool is_done = end == m_values.size();

    for (auto i = begin; i < end; ++i) {
      m_values[i].value += m_values[i].delta.exchange(
          0, memory_order_relaxed);
    }

    return !is_done;
  }

  template <typename int_, atomic_integer atomic_int_>
  void basic_buffer<int_, atomic_int_>::adjust_done() noexcept {
    if (is_error())
      return;

    m_next_chunk.store(0, memory_order_relaxed);
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::_error() const noexcept
      -> basic_buffer<int_, atomic_int_> {
    auto result       = basic_buffer<int_, atomic_int_> {};
    result.m_is_error = true;
    return result;
  }

  template <typename int_, atomic_integer atomic_int_>
  basic_buffer<int_, atomic_int_>::row::row(row const &r) noexcept :
      empty(r.empty), offset(r.offset), value(r.value),
      delta(r.delta.load(std::memory_order_relaxed)) { }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::row::operator=(
      row const &r) noexcept -> row & {
    empty  = r.empty;
    offset = r.offset;
    value  = r.value;
    delta.store(r.delta.load(std::memory_order_relaxed),
                memory_order_relaxed);
    return *this;
  }
}

#endif
