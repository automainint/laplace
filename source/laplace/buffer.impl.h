#ifndef LAPLACE_BUFFER_IMPL_H
#define LAPLACE_BUFFER_IMPL_H

#include "buffer.h"

#include <limits>
#include <stdexcept>

namespace laplace {
  using std::bad_alloc, std::numeric_limits,
      std::memory_order_relaxed, std::min, std::max;

  template <typename int_, atomic_integer atomic_int_>
  basic_buffer<int_, atomic_int_>::basic_buffer(
      basic_buffer const &buf) noexcept :
      m_is_error(buf.m_is_error),
      m_chunk_size(buf.m_chunk_size), m_reserved(buf.m_reserved),
      m_next_block(buf.m_next_block),
      m_next_chunk(buf.m_next_chunk.load(memory_order_relaxed)),
      m_blocks(buf.m_blocks), m_values(buf.m_values) { }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::operator=(
      basic_buffer const &buf) noexcept -> basic_buffer & {
    m_is_error   = buf.m_is_error;
    m_chunk_size = buf.m_chunk_size;
    m_reserved   = buf.m_reserved;
    m_next_block = buf.m_next_block;
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
  auto basic_buffer<int_, atomic_int_>::error() const noexcept
      -> bool {
    return m_is_error;
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::set_chunk_size(ptrdiff_t size)
      const noexcept -> basic_buffer<int_, atomic_int_> {
    return _bind([&]() {
      if (size <= 0)
        return _error();

      auto result         = *this;
      result.m_chunk_size = size;
      return result;
    });
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::chunk_size() const noexcept
      -> ptrdiff_t {
    return m_chunk_size;
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::size() const noexcept
      -> ptrdiff_t {
    return _bind<ptrdiff_t>([&]() { return m_values.size(); },
                            index_undefined);
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::reserve(
      ptrdiff_t count) noexcept -> bool {
    return _bind<bool>(
        [&]() {
          if (count < 0)
            return false;

          if (m_blocks.size() < count) {
            try {
              m_blocks.resize(count, index_undefined);
            } catch (bad_alloc &) { return false; }
          }

          m_reserved   = count;
          m_next_block = max(m_next_block, count);
          while (m_next_block < m_blocks.size() &&
                 m_blocks[m_next_block] != index_undefined)
            ++m_next_block;
          return true;
        },
        false);
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::allocate(
      ptrdiff_t size) noexcept -> ptrdiff_t {
    return _bind<ptrdiff_t>(
        [&]() {
          if (size <= 0)
            return id_undefined;

          auto const offset = _alloc(size);
          if (offset == index_undefined)
            return id_undefined;

          auto const block = m_next_block;
          if (block >= m_blocks.size()) {
            try {
              m_blocks.resize(block + 1, index_undefined);
            } catch (bad_alloc &) {
              _free(offset);
              return id_undefined;
            }
          }
          m_blocks[block] = offset;

          while (m_next_block < m_blocks.size() &&
                 m_blocks[m_next_block] != index_undefined)
            ++m_next_block;

          return block;
        },
        id_undefined);
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::allocate_into(
      ptrdiff_t block, ptrdiff_t size) noexcept -> bool {
    return _bind<bool>(
        [&]() {
          if (block < 0)
            return false;
          if (size <= 0)
            return false;

          if (block < m_blocks.size() &&
              m_blocks[block] != index_undefined)
            _free(m_blocks[block]);

          auto const offset = _alloc(size);
          if (offset == index_undefined)
            return false;

          if (block >= m_blocks.size()) {
            try {
              m_blocks.resize(block + 1, index_undefined);
            } catch (bad_alloc &) {
              _free(offset);
              return false;
            }
          }

          m_blocks[block] = offset;

          while (m_next_block < m_blocks.size() &&
                 m_blocks[m_next_block] != index_undefined)
            ++m_next_block;

          return true;
        },
        false);
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::deallocate(
      ptrdiff_t block) noexcept -> bool {
    return _bind<bool>(
        [&]() {
          if (block < 0 || block >= m_blocks.size())
            return false;
          if (m_blocks[block] == index_undefined)
            return false;

          _free(m_blocks[block]);

          m_blocks[block] = index_undefined;
          m_next_block    = max(m_reserved, min(m_next_block, block));
          return true;
        },
        false);
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::get(ptrdiff_t block,
                                            ptrdiff_t index,
                                            int_ fail) const noexcept
      -> int_ {
    return _bind<int_>(
        [&]() {
          if (block < 0 || block >= m_blocks.size())
            return fail;
          auto const offset = m_blocks[block] + index;
          if (offset < 0 || offset >= m_values.size())
            return fail;
          return m_values[offset].value;
        },
        fail);
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::set(ptrdiff_t block,
                                            ptrdiff_t index,
                                            int_      value) noexcept
      -> bool {
    return _bind<bool>(
        [&]() {
          if (block < 0 || block >= m_blocks.size())
            return false;
          auto const offset = m_blocks[block] + index;
          if (offset < 0 || offset >= m_values.size())
            return false;
          m_values[offset].delta.fetch_add(
              value - m_values[offset].value, memory_order_relaxed);
          return true;
        },
        false);
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::add(ptrdiff_t block,
                                            ptrdiff_t index,
                                            int_      delta) noexcept
      -> bool {
    return _bind<bool>(
        [&]() {
          if (block < 0 || block >= m_blocks.size())
            return false;
          auto const offset = m_blocks[block] + index;
          if (offset < 0 || offset >= m_values.size())
            return false;
          m_values[offset].delta.fetch_add(delta,
                                           memory_order_relaxed);
          return true;
        },
        false);
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::adjust() noexcept -> bool {
    return _bind<bool>(
        [&]() {
          const auto begin = m_next_chunk.fetch_add(
              m_chunk_size, memory_order_relaxed);
          const auto end     = min<ptrdiff_t>(begin + m_chunk_size,
                                          m_values.size());
          const bool is_done = end == m_values.size();

          for (auto i = begin; i < end; ++i) {
            m_values[i].value += m_values[i].delta.exchange(
                0, memory_order_relaxed);
          }

          return !is_done;
        },
        false);
  }

  template <typename int_, atomic_integer atomic_int_>
  void basic_buffer<int_, atomic_int_>::adjust_done() noexcept {
    if (!error())
      m_next_chunk.store(0, memory_order_relaxed);
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::_error() const noexcept
      -> basic_buffer<int_, atomic_int_> {
    auto result       = basic_buffer<int_, atomic_int_> { *this };
    result.m_is_error = true;
    return result;
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::_bind(
      std::function<basic_buffer<int_, atomic_int_>()> f)
      const noexcept -> basic_buffer<int_, atomic_int_> {
    if (error())
      return _error();
    return f();
  }

  template <typename int_, atomic_integer atomic_int_>
  template <typename type_>
  auto basic_buffer<int_, atomic_int_>::_bind(
      std::function<type_()> f, type_ def) const noexcept -> type_ {
    if (error())
      return def;
    return f();
  }

  template <typename int_, atomic_integer atomic_int_>
  auto basic_buffer<int_, atomic_int_>::_alloc(
      ptrdiff_t size) noexcept -> ptrdiff_t {
    auto offset = ptrdiff_t {};

    for (; offset < m_values.size();
         offset += m_values[offset].offset) {
      if (m_values[offset].empty && m_values[offset].offset >= size)
        break;
    }

    if (size >= numeric_limits<ptrdiff_t>::max() - offset)
      return index_undefined;

    if (offset >= m_values.size()) {
      try {
        m_values.resize(offset + size);
      } catch (bad_alloc &) { return index_undefined; }
    }

    for (auto i = 0; i < size; ++i) {
      m_values[offset + i].empty  = false;
      m_values[offset + i].offset = size - i;
    }

    return offset;
  }

  template <typename int_, atomic_integer atomic_int_>
  void basic_buffer<int_, atomic_int_>::_free(
      ptrdiff_t offset) noexcept {
    auto begin = offset;
    auto end   = begin;
    while (end < m_values.size()) {
      end += m_values[end].offset;
      if (end < m_values.size() && !m_values[end].empty)
        break;
    }
    for (auto i = begin; i < end; ++i) {
      m_values[i].empty  = true;
      m_values[i].offset = end - i;
      m_values[i].value  = 0;
      m_values[i].delta.store(0, memory_order_relaxed);
    }
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
