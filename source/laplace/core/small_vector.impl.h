/*  laplace/core/small_vector.impl.h
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

#ifndef laplace_core_small_vector_impl_h
#define laplace_core_small_vector_impl_h

namespace laplace {
  template <typename elem_, ptrdiff_t small_size_>
  inline small_vector<elem_, small_size_>::small_vector() noexcept {
    m_values = m_static_data;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline small_vector<elem_, small_size_>::small_vector(
      small_vector &&v) noexcept {
    m_capacity = v.m_capacity;
    m_size     = v.m_size;

    if (m_capacity <= small_count) {
      m_values = m_static_data;
      std::move(v.m_static_data, v.m_static_data + small_count,
                m_static_data);
    } else {
      m_values     = v.m_values;
      v.m_capacity = 0;
    }
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline small_vector<elem_, small_size_>::small_vector(
      const small_vector<elem_, small_size_> &v) noexcept {
    m_capacity = v.m_capacity;
    m_size     = v.m_size;

    if (m_capacity <= small_count) {
      m_values = m_static_data;
    } else {
      m_values = new elem_[m_capacity];
    }

    std::copy(v.m_values, v.m_values + m_size, m_values);
  }

  template <typename elem_, ptrdiff_t small_size_>
  template <ptrdiff_t size2_>
  inline small_vector<elem_, small_size_>::small_vector(
      const small_vector<elem_, size2_> &v) noexcept {
    m_capacity = v.m_capacity;
    m_size     = v.m_size;

    if (m_capacity <= small_count) {
      m_values = m_static_data;
    } else {
      m_values = new elem_[m_capacity];
    }

    std::copy(v.m_values, v.m_values + m_size, m_values);
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline small_vector<elem_, small_size_>::small_vector(
      const ptrdiff_t size, const elem_ value) {
    m_values = m_static_data;
    reserve(size);

    std::fill(m_values, m_values + size, value);
    m_size = size;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline small_vector<elem_, small_size_>::small_vector(
      std::initializer_list<elem_> values) noexcept {
    m_values = m_static_data;
    reserve(values.size());

    std::copy(values.begin(), values.end(), m_values);
    m_size = values.size();
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline small_vector<elem_, small_size_>::~small_vector() noexcept {
    if (m_capacity > small_count) {
      delete[] m_values;
    }
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::operator=(
      small_vector &&v) noexcept -> small_vector & {

    if (m_capacity > small_count) {
      delete[] m_values;
    }

    m_capacity = v.m_capacity;
    m_size     = v.m_size;

    if (m_capacity <= small_count) {
      m_values = m_static_data;
      std::move(v.m_static_data, v.m_static_data + small_count,
                m_static_data);
    } else {
      m_values     = v.m_values;
      v.m_capacity = 0;
    }

    return *this;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::operator=(
      const small_vector<elem_, small_size_> &v) noexcept
      -> small_vector & {

    if (m_capacity < v.m_size) {
      if (m_capacity > small_count) {
        delete[] m_values;
      }

      m_capacity = std::bit_ceil(static_cast<size_t>(v.m_size));
      m_values   = new elem_[m_capacity];
    }

    std::copy(v.m_values, v.m_values + v.m_size, m_values);
    m_size = v.m_size;

    return *this;
  }

  template <typename elem_, ptrdiff_t small_size_>
  template <ptrdiff_t size2_>
  inline auto small_vector<elem_, small_size_>::operator=(
      const small_vector<elem_, size2_> &v) noexcept
      -> small_vector & {

    if (m_capacity < v.m_size) {
      if (m_capacity > small_count) {
        delete[] m_values;
      }

      m_capacity = std::bit_ceil(static_cast<size_t>(v.m_size));
      m_values   = new elem_[m_capacity];
    }

    std::copy(v.m_values, v.m_values + v.m_size, m_values);
    m_size = v.m_size;

    return *this;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::empty() const noexcept
      -> bool {
    return m_size <= 0;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::size() const noexcept
      -> ptrdiff_t {
    return m_size;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::capacity()
      const noexcept -> ptrdiff_t {
    return m_capacity;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline void small_vector<elem_, small_size_>::push_back(
      const elem_ &value) noexcept {
    reserve(m_size + 1);

    m_values[m_size] = value;
    m_size++;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline void small_vector<elem_, small_size_>::emplace_back(
      elem_ &&value) noexcept {
    reserve(m_size + 1);

    m_values[m_size] = value;
    m_size++;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline void small_vector<elem_, small_size_>::pop_back() noexcept {
    resize(m_size - 1);
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline void small_vector<elem_, small_size_>::insert(
      const_iterator position, const elem_ &value) noexcept {

    const auto p = position - m_values;

    reserve(m_size + 1);

    std::move_backward(m_values + p, m_values + m_size,
                       m_values + m_size + 1);

    m_values[p] = value;
    m_size++;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline void small_vector<elem_, small_size_>::emplace(
      const_iterator position, elem_ &&value) noexcept {

    const auto p = position - m_values;

    reserve(m_size + 1);

    std::move_backward(m_values + p, m_values + m_size,
                       m_values + m_size + 1);

    m_values[p] = value;
    m_size++;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline void small_vector<elem_, small_size_>::reserve(
      const ptrdiff_t size) {

    if (size < m_capacity) {
      return;
    }

    const auto  n = std::bit_ceil(static_cast<size_t>(size));
    auto *const p = new elem_[n];

    std::move(m_values, m_values + m_size, p);

    if (m_capacity > small_count) {
      delete[] m_values;
    }

    m_values   = p;
    m_capacity = n;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline void small_vector<elem_, small_size_>::resize(
      const ptrdiff_t size, const elem_ value) noexcept {

    reserve(size);

    if (m_size < size) {
      std::fill(m_values + m_size, m_values + size, value);
    }

    m_size = size;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline void small_vector<elem_, small_size_>::clear() noexcept {
    m_size = 0;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::at(
      const ptrdiff_t n) noexcept -> elem_ & {
    return m_values[n];
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::at(
      const ptrdiff_t n) const noexcept -> const elem_ & {
    return m_values[n];
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::front() noexcept
      -> elem_ & {
    return at(0);
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::front() const noexcept
      -> const elem_ & {
    return at(0);
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::back() noexcept
      -> elem_ & {
    return at(m_size - 1);
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::back() const noexcept
      -> const elem_ & {
    return at(m_size - 1);
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::data() noexcept
      -> elem_ * {
    return m_values;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::data() const noexcept
      -> const elem_ * {
    return m_values;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::begin() noexcept
      -> iterator {
    return m_values;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::end() noexcept
      -> iterator {
    return m_values + m_size;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::begin() const noexcept
      -> const_iterator {
    return cbegin();
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::end() const noexcept
      -> const_iterator {
    return cend();
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::cbegin()
      const noexcept -> const_iterator {
    return m_values;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::cend() const noexcept
      -> const_iterator {
    return m_values + m_size;
  }

  template <typename elem_, ptrdiff_t small_size_>
  template <ptrdiff_t size2_>
  inline auto small_vector<elem_, small_size_>::compare(
      const small_vector<elem_, size2_> &v) const noexcept -> int {

    const auto n = std::min(m_size, v.m_size);

    for (ptrdiff_t i = 0; i < n; i++) {
      if (m_values[i] < v.m_values[i])
        return -1;
      if (m_values[i] > v.m_values[i])
        return 1;
    }

    if (m_size < v.m_size)
      return -1;
    if (m_size > v.m_size)
      return 1;

    return 0;
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::operator[](
      const ptrdiff_t n) noexcept -> elem_ & {
    return at(n);
  }

  template <typename elem_, ptrdiff_t small_size_>
  inline auto small_vector<elem_, small_size_>::operator[](
      const ptrdiff_t n) const noexcept -> const elem_ & {
    return at(n);
  }

  template <typename elem_, ptrdiff_t size1_, ptrdiff_t size2_>
  inline auto operator==(
      const small_vector<elem_, size1_> &a,
      const small_vector<elem_, size2_> &b) noexcept -> bool {
    return a.compare(b) == 0;
  }

  template <typename elem_, ptrdiff_t size1_, ptrdiff_t size2_>
  inline auto operator!=(
      const small_vector<elem_, size1_> &a,
      const small_vector<elem_, size2_> &b) noexcept -> bool {
    return a.compare(b) != 0;
  }

  template <typename elem_, ptrdiff_t size1_, ptrdiff_t size2_>
  inline auto operator<=(
      const small_vector<elem_, size1_> &a,
      const small_vector<elem_, size2_> &b) noexcept -> bool {
    return a.compare(b) <= 0;
  }

  template <typename elem_, ptrdiff_t size1_, ptrdiff_t size2_>
  inline auto operator>=(
      const small_vector<elem_, size1_> &a,
      const small_vector<elem_, size2_> &b) noexcept -> bool {
    return a.compare(b) >= 0;
  }

  template <typename elem_, ptrdiff_t size1_, ptrdiff_t size2_>
  inline auto operator<(const small_vector<elem_, size1_> &a,
                        const small_vector<elem_, size2_> &b) noexcept
      -> bool {
    return a.compare(b) < 0;
  }

  template <typename elem_, ptrdiff_t size1_, ptrdiff_t size2_>
  inline auto operator>(const small_vector<elem_, size1_> &a,
                        const small_vector<elem_, size2_> &b) noexcept
      -> bool {
    return a.compare(b) > 0;
  }
}

#endif
