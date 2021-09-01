/*  laplace/core/small_vector.h
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

#ifndef laplace_core_small_vector_h
#define laplace_core_small_vector_h

#include <algorithm>
#include <bit>
#include <cstddef>

namespace laplace {
  template <typename elem_, ptrdiff_t small_size_ = 80>
  class small_vector {
  public:
    using iterator       = elem_ *;
    using const_iterator = const elem_ *;

    static constexpr ptrdiff_t small_count = small_size_ /
                                             sizeof(elem_);

    small_vector() noexcept;
    small_vector(small_vector &&v) noexcept;
    small_vector(const small_vector<elem_, small_size_> &v) noexcept;

    template <ptrdiff_t size2_>
    small_vector(const small_vector<elem_, size2_> &v) noexcept;

    small_vector(const ptrdiff_t size, const elem_ value = elem_ {});
    small_vector(std::initializer_list<elem_> values) noexcept;

    ~small_vector() noexcept;

    auto operator=(small_vector &&v) noexcept -> small_vector &;
    auto operator=(const small_vector<elem_, small_size_> &v) noexcept
        -> small_vector &;

    template <ptrdiff_t size2_>
    auto operator=(const small_vector<elem_, size2_> &v) noexcept
        -> small_vector &;

    auto empty() const noexcept -> bool;
    auto size() const noexcept -> ptrdiff_t;
    auto capacity() const noexcept -> ptrdiff_t;

    void push_back(const elem_ &value) noexcept;
    void emplace_back(elem_ &&value) noexcept;
    void pop_back() noexcept;

    void insert(const_iterator position, const elem_ &value) noexcept;
    void emplace(const_iterator position, elem_ &&value) noexcept;
    void reserve(const ptrdiff_t size);

    void resize(const ptrdiff_t size,
                const elem_     value = elem_ {}) noexcept;

    void clear() noexcept;

    auto at(const ptrdiff_t n) noexcept -> elem_ &;
    auto at(const ptrdiff_t n) const noexcept -> const elem_ &;

    auto front() noexcept -> elem_ &;
    auto front() const noexcept -> const elem_ &;

    auto back() noexcept -> elem_ &;
    auto back() const noexcept -> const elem_ &;

    auto data() noexcept -> elem_ *;
    auto data() const noexcept -> const elem_ *;

    auto begin() noexcept -> iterator;
    auto end() noexcept -> iterator;

    auto begin() const noexcept -> const_iterator;
    auto end() const noexcept -> const_iterator;

    auto cbegin() const noexcept -> const_iterator;
    auto cend() const noexcept -> const_iterator;

    template <ptrdiff_t size2_>
    auto compare(const small_vector<elem_, size2_> &v) const noexcept
        -> int;

    auto operator[](const ptrdiff_t n) noexcept -> elem_ &;
    auto operator[](const ptrdiff_t n) const noexcept
        -> const elem_ &;

  private:
    ptrdiff_t m_capacity                 = small_count;
    ptrdiff_t m_size                     = {};
    elem_ *   m_values                   = nullptr;
    elem_     m_static_data[small_count] = {};
  };

  template <typename elem_, ptrdiff_t size1_, ptrdiff_t size2_>
  auto operator==(const small_vector<elem_, size1_> &a,
                  const small_vector<elem_, size2_> &b) noexcept
      -> bool;

  template <typename elem_, ptrdiff_t size1_, ptrdiff_t size2_>
  auto operator!=(const small_vector<elem_, size1_> &a,
                  const small_vector<elem_, size2_> &b) noexcept
      -> bool;

  template <typename elem_, ptrdiff_t size1_, ptrdiff_t size2_>
  auto operator<=(const small_vector<elem_, size1_> &a,
                  const small_vector<elem_, size2_> &b) noexcept
      -> bool;

  template <typename elem_, ptrdiff_t size1_, ptrdiff_t size2_>
  auto operator>=(const small_vector<elem_, size1_> &a,
                  const small_vector<elem_, size2_> &b) noexcept
      -> bool;

  template <typename elem_, ptrdiff_t size1_, ptrdiff_t size2_>
  auto operator<(const small_vector<elem_, size1_> &a,
                 const small_vector<elem_, size2_> &b) noexcept
      -> bool;

  template <typename elem_, ptrdiff_t size1_, ptrdiff_t size2_>
  auto operator>(const small_vector<elem_, size1_> &a,
                 const small_vector<elem_, size2_> &b) noexcept
      -> bool;
}

#include "small_vector.impl.h"

#endif
