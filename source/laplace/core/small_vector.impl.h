/*  Copyright (c) 2022 Mitya Selivanov
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
  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline small_vector<elem_, small_size_,
                      unsafe_>::small_vector() noexcept :
      m_values(_static_data()) { }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline small_vector<elem_, small_size_, unsafe_>::small_vector(
      small_vector &&v) noexcept :
      m_capacity(v.m_capacity),
      m_size(v.m_size) {

    if (m_capacity <= small_count) {
      m_values = _static_data();

      _move(v.m_values, v.m_values + m_size, m_values);

    } else {
      m_values     = v.m_values;
      v.m_capacity = 0;
      v.m_size     = 0;
    }
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline small_vector<elem_, small_size_, unsafe_>::small_vector(
      small_vector const &v) noexcept {
    _assign(v.m_values, v.m_values + v.m_size);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  template <ptrdiff_t size2_, bool unsafe2_>
  inline small_vector<elem_, small_size_, unsafe_>::small_vector(
      small_vector<elem_, size2_, unsafe2_> const &v) noexcept {
    _assign(v.begin(), v.end());
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline small_vector<elem_, small_size_, unsafe_>::small_vector(
      const ptrdiff_t size, const elem_ value) noexcept :
      m_values(_static_data()) {

    reserve(size);
    _fill(m_values, m_values + size, value);

    m_size = size;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline small_vector<elem_, small_size_, unsafe_>::small_vector(
      const ptrdiff_t size, auto &&...args) noexcept :
      m_values(_static_data()) {

    reserve(size);
    _fill(m_values, m_values + size, args...);

    m_size = size;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline small_vector<elem_, small_size_, unsafe_>::small_vector(
      std::initializer_list<elem_> values) noexcept :
      m_values(_static_data()) {

    reserve(values.size());
    _copy(values.begin(), values.end(), m_values);

    m_size = values.size();
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline small_vector<elem_, small_size_,
                      unsafe_>::~small_vector() noexcept {
    _free(m_values, m_values + m_size);

    if (m_capacity > small_count)
      _dealloc(m_values);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::operator=(
      small_vector &&v) noexcept -> small_vector & {

    _free(m_values, m_values + m_size);

    if (m_capacity > small_count)
      _dealloc(m_values);

    m_capacity = v.m_capacity;
    m_size     = v.m_size;

    if (m_capacity <= small_count) {
      m_values = _static_data();

      _move(v.m_values, v.m_values + m_size, m_values);

    } else {
      m_values     = v.m_values;
      v.m_capacity = 0;
      v.m_size     = 0;
    }

    return *this;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::operator=(
      const small_vector<elem_, small_size_, unsafe_> &v) noexcept
      -> small_vector & {

    reserve(v.m_size);

    std::copy(v.m_values, v.m_values + m_size, m_values);
    _copy(v.m_values + m_size, v.m_values + v.m_size,
          m_values + m_size);

    m_size = v.m_size;

    return *this;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  template <ptrdiff_t size2_, bool unsafe2_>
  inline auto small_vector<elem_, small_size_, unsafe_>::operator=(
      const small_vector<elem_, size2_, unsafe2_> &v) noexcept
      -> small_vector & {

    reserve(v.size());

    std::copy(v.data(), v.data() + m_size, m_values);
    _copy(v.data() + m_size, v.data() + v.size(), m_values + m_size);

    m_size = v.size();

    return *this;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::empty()
      const noexcept -> bool {
    return m_size <= 0;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::size()
      const noexcept -> ptrdiff_t {
    return m_size;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::capacity()
      const noexcept -> ptrdiff_t {
    return m_capacity;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::push_back(
      const elem_ &value) noexcept {
    reserve(m_size + 1);
    _fill_one(m_values + m_size, value);
    m_size++;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void
  small_vector<elem_, small_size_, unsafe_>::pop_back() noexcept {
    resize(m_size - 1);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::insert(
      const_iterator position, const elem_ &value) noexcept {

    if constexpr (!unsafe_) {
      if (position < m_values || position > m_values + m_size)
        return;
    }

    reserve(m_size + 1);

    _fill_one(m_values + m_size, m_values[m_size - 1]);

    auto const p = position - m_values;
    std::move_backward(m_values + p, m_values + m_size - 1,
                       m_values + m_size);
    m_values[p] = value;

    m_size++;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::insert(
      const_iterator position, auto begin, auto end) noexcept {

    if constexpr (!unsafe_) {
      if (position < m_values || position > m_values + m_size)
        return;
      if (end < begin)
        return;
    }

    auto const p = position - m_values;
    auto const s = end - begin;
    auto const x = m_size - p;

    reserve(m_size + s);

    if (m_size <= p + s) {
      _move_backward(m_values + p, m_values + m_size,
                     m_values + m_size + s);
      _copy(begin + x, end, m_values + m_size);
      std::copy(begin, begin + x, m_values + p);

    } else {
      _move_backward(m_values + p + x, m_values + m_size,
                     m_values + m_size + s);
      std::move_backward(m_values + p, m_values + p + x,
                         m_values + m_size);
      std::copy(begin, end, m_values + p);
    }

    m_size += s;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::emplace(
      const_iterator position, auto &&...args) noexcept {

    if constexpr (!unsafe_) {
      if (position < m_values || position > m_values + m_size)
        return;
    }

    reserve(m_size + 1);

    auto const p = position - m_values;

    if (p < m_size) {
      _fill_one(m_values + m_size, m_values[m_size - 1]);
      std::move_backward(m_values + p, m_values + m_size - 1,
                         m_values + m_size);
      m_values[p] = elem_(args...);
    } else {
      _fill_one(m_values + m_size, args...);
    }

    m_size++;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::emplace_back(
      auto &&...args) noexcept {
    reserve(m_size + 1);
    _fill_one(m_values + m_size, args...);
    m_size++;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::erase(
      const_iterator position) noexcept {
    if constexpr (!unsafe_) {
      if (position < m_values || position >= m_values + m_size)
        return;
    }

    auto const p = position - m_values;
    std::move(m_values + p + 1, m_values + m_size, m_values + p);
    m_size--;
    _free_one(m_values + m_size);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::erase(
      const_iterator begin, const_iterator end) noexcept {

    if constexpr (!unsafe_) {
      if (end < begin || begin < m_values || end >= m_values + m_size)
        return;
    }

    auto const p = begin - m_values;
    auto const s = end - begin;

    std::move(m_values + p + s, m_values + m_size, m_values + p);
    _free(m_values + m_size - s, m_values + m_size);

    m_size -= s;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::reserve(
      const ptrdiff_t size) {

    if constexpr (!unsafe_) {
      if (size < 0)
        return;
    }

    if (size < m_capacity) {
      return;
    }

    auto const  n = std::bit_ceil(static_cast<size_t>(size));
    auto *const p = _alloc(n);

    if (p == nullptr)
      return;

    _move(m_values, m_values + m_size, p);
    _free(m_values, m_values + m_size);

    if (m_capacity > small_count) {
      _dealloc(m_values);
    }

    m_values   = p;
    m_capacity = n;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::resize(
      const ptrdiff_t size, const elem_ value) noexcept {

    reserve(size);
    _fill(m_values + m_size, m_values + size, value);
    m_size = size;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void
  small_vector<elem_, small_size_, unsafe_>::clear() noexcept {
    _free(m_values, m_values + m_size);
    m_size = 0;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::at(
      const ptrdiff_t n) noexcept -> elem_ & {

    if constexpr (!unsafe_) {
      if (n < 0 || n >= m_size) {
        static auto x = elem_ {};
        return x;
      }
    }

    return m_values[n];
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::at(
      const ptrdiff_t n) const noexcept -> const elem_ & {

    if constexpr (!unsafe_) {
      if (n < 0 || n >= m_size) {
        static auto const x = elem_ {};
        return x;
      }
    }

    return m_values[n];
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto
  small_vector<elem_, small_size_, unsafe_>::front() noexcept
      -> elem_ & {
    return at(0);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::front()
      const noexcept -> const elem_ & {
    return at(0);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto
  small_vector<elem_, small_size_, unsafe_>::back() noexcept
      -> elem_ & {
    return at(m_size - 1);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::back()
      const noexcept -> const elem_ & {
    return at(m_size - 1);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto
  small_vector<elem_, small_size_, unsafe_>::data() noexcept
      -> elem_ * {
    return m_values;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::data()
      const noexcept -> const elem_ * {
    return m_values;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto
  small_vector<elem_, small_size_, unsafe_>::begin() noexcept
      -> iterator {
    return m_values;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto
  small_vector<elem_, small_size_, unsafe_>::end() noexcept
      -> iterator {
    return m_values + m_size;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::begin()
      const noexcept -> const_iterator {
    return cbegin();
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::end()
      const noexcept -> const_iterator {
    return cend();
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::cbegin()
      const noexcept -> const_iterator {
    return m_values;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::cend()
      const noexcept -> const_iterator {
    return m_values + m_size;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  template <ptrdiff_t size2_, bool unsafe2_>
  inline auto small_vector<elem_, small_size_, unsafe_>::compare(
      const small_vector<elem_, size2_, unsafe2_> &v) const noexcept
      -> int {

    auto const n = std::min(m_size, v.size());

    for (ptrdiff_t i = 0; i < n; i++) {
      if (m_values[i] < v.at(i))
        return -1;
      if (m_values[i] > v.at(i))
        return 1;
    }

    if (m_size < v.size())
      return -1;
    if (m_size > v.size())
      return 1;

    return 0;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::operator[](
      const ptrdiff_t n) noexcept -> elem_ & {
    return at(n);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::operator[](
      const ptrdiff_t n) const noexcept -> const elem_ & {
    return at(n);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto
  small_vector<elem_, small_size_, unsafe_>::_static_data() noexcept
      -> elem_ * {
    return reinterpret_cast<elem_ *>(m_static_data);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline auto small_vector<elem_, small_size_, unsafe_>::_alloc(
      const ptrdiff_t count) noexcept -> elem_ * {

    auto *p = new (std::nothrow) char[count * sizeof(elem_)];

    if (p == nullptr)
      return nullptr;

    return reinterpret_cast<elem_ *>(p);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::_dealloc(
      elem_ *p) noexcept {

    if constexpr (!unsafe_) {
      if (p == nullptr)
        return;
    }

    delete[] reinterpret_cast<char *>(p);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::_assign(
      auto const *begin, auto const *end) {

    if constexpr (!unsafe_) {
      if (end < begin)
        return;
    }

    m_capacity = std::bit_ceil(static_cast<size_t>(end - begin));

    if (m_capacity <= small_count) {
      m_values   = _static_data();
      m_capacity = small_count;

    } else {
      auto *p = _alloc(m_capacity);

      if (p == nullptr) {
        m_values   = _static_data();
        m_capacity = small_count;
        return;
      }

      m_values = p;
    }

    _copy(begin, end, m_values);

    m_size = end - begin;
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::_copy(
      auto const *begin, auto const *end, elem_ *dst) noexcept {
    while (begin < end) { new (dst++) elem_(*(begin++)); }
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::_move(
      elem_ *begin, const elem_ *end, elem_ *dst) noexcept {
    while (begin < end) { new (dst++) elem_(std::move(*(begin++))); }
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void
  small_vector<elem_, small_size_, unsafe_>::_move_backward(
      auto const *begin, auto const *end, elem_ *dst_end) noexcept {
    while (begin < end) {
      new (--dst_end) elem_(std::move(*(--end)));
    }
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::_free_one(
      elem_ *position) noexcept {
    position->~elem_();
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::_free(
      elem_ *begin, const elem_ *end) noexcept {
    while (begin < end) { _free_one(begin++); }
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::_fill_one(
      elem_ *position, auto &&...args) noexcept {
    new (position) elem_(args...);
  }

  template <typename elem_, ptrdiff_t small_size_, bool unsafe_>
  inline void small_vector<elem_, small_size_, unsafe_>::_fill(
      elem_ *begin, const elem_ *end, auto &&...args) noexcept {
    while (begin < end) { _fill_one(begin++, args...); }
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
