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

#include "log.h"
#include <algorithm>
#include <bit>
#include <cstddef>
#include <memory>

namespace laplace {
  template <typename elem_,
            ptrdiff_t small_size_ = 80,
            bool      unsafe_     = false>
  class small_vector final {
  public:
    static_assert(small_size_ > 0);

    using iterator       = elem_ *;
    using const_iterator = const elem_ *;

    static constexpr ptrdiff_t small_count = (small_size_ +
                                              sizeof(elem_) - 1) /
                                             sizeof(elem_);

    static constexpr ptrdiff_t static_size = small_count *
                                             sizeof(elem_);

    small_vector() noexcept;
    small_vector(small_vector &&v) noexcept;
    small_vector(const small_vector &v) noexcept;

    template <ptrdiff_t size2_, bool unsafe2_>
    small_vector(
        const small_vector<elem_, size2_, unsafe2_> &v) noexcept;

    small_vector(const ptrdiff_t size, const elem_ value) noexcept;
    small_vector(const ptrdiff_t size, auto &&...args) noexcept;
    small_vector(std::initializer_list<elem_> values) noexcept;

    ~small_vector() noexcept;

    auto operator=(small_vector &&v) noexcept -> small_vector &;
    auto operator=(const small_vector &v) noexcept -> small_vector &;

    template <ptrdiff_t size2_, bool unsafe2_>
    auto operator=(
        const small_vector<elem_, size2_, unsafe2_> &v) noexcept
        -> small_vector &;

    [[nodiscard]] auto empty() const noexcept -> bool;
    [[nodiscard]] auto size() const noexcept -> ptrdiff_t;
    [[nodiscard]] auto capacity() const noexcept -> ptrdiff_t;

    void push_back(const elem_ &value) noexcept;
    void pop_back() noexcept;

    void insert(const_iterator position, const elem_ &value) noexcept;
    void insert(const_iterator position,
                auto           begin,
                auto           end) noexcept;

    void emplace(const_iterator position, auto &&...args) noexcept;
    void emplace_back(auto &&...args) noexcept;

    void erase(const_iterator position) noexcept;
    void erase(const_iterator begin, const_iterator end) noexcept;

    void reserve(const ptrdiff_t size);

    void resize(const ptrdiff_t size,
                const elem_     value = elem_ {}) noexcept;

    void clear() noexcept;

    [[nodiscard]] auto at(const ptrdiff_t n) noexcept -> elem_ &;
    [[nodiscard]] auto at(const ptrdiff_t n) const noexcept
        -> const elem_ &;

    [[nodiscard]] auto front() noexcept -> elem_ &;
    [[nodiscard]] auto front() const noexcept -> const elem_ &;

    [[nodiscard]] auto back() noexcept -> elem_ &;
    [[nodiscard]] auto back() const noexcept -> const elem_ &;

    [[nodiscard]] auto data() noexcept -> elem_ *;
    [[nodiscard]] auto data() const noexcept -> const elem_ *;

    [[nodiscard]] auto begin() noexcept -> iterator;
    [[nodiscard]] auto end() noexcept -> iterator;

    [[nodiscard]] auto begin() const noexcept -> const_iterator;
    [[nodiscard]] auto end() const noexcept -> const_iterator;

    [[nodiscard]] auto cbegin() const noexcept -> const_iterator;
    [[nodiscard]] auto cend() const noexcept -> const_iterator;

    template <ptrdiff_t size2_, bool unsafe_>
    [[nodiscard]] auto compare(
        const small_vector<elem_, size2_, unsafe_> &v) const noexcept
        -> int;

    [[nodiscard]] auto operator[](const ptrdiff_t n) noexcept
        -> elem_ &;
    [[nodiscard]] auto operator[](const ptrdiff_t n) const noexcept
        -> const elem_ &;

  private:
    [[nodiscard]] auto _static_data() noexcept -> elem_ *;
    [[nodiscard]] auto _alloc(const ptrdiff_t count) noexcept
        -> elem_ *;
    void _dealloc(elem_ *p) noexcept;
    void _assign(const auto *begin, const auto *end);
    void _copy(const auto *begin,
               const auto *end,
               elem_ *     dst) noexcept;
    void _move(elem_ *begin, const elem_ *end, elem_ *dst) noexcept;
    void _move_backward(const auto *begin,
                        const auto *end,
                        elem_ *     dst_end) noexcept;
    void _free_one(elem_ *position) noexcept;
    void _fill_one(elem_ *position, auto &&...args) noexcept;
    void _free(elem_ *begin, const elem_ *end) noexcept;
    void _fill(elem_ *      begin,
               const elem_ *end,
               auto &&...args) noexcept;

    ptrdiff_t m_capacity                 = small_count;
    ptrdiff_t m_size                     = {};
    elem_ *   m_values                   = nullptr;
    char      m_static_data[static_size] = {};
  };

  template <typename elem_,
            ptrdiff_t size1_,
            bool      unsafe1_,
            ptrdiff_t size2_,
            bool      unsafe2_>
  [[nodiscard]] auto operator==(
      const small_vector<elem_, size1_, unsafe1_> &a,
      const small_vector<elem_, size2_, unsafe2_> &b) noexcept
      -> bool;

  template <typename elem_,
            ptrdiff_t size1_,
            bool      unsafe1_,
            ptrdiff_t size2_,
            bool      unsafe2_>
  [[nodiscard]] auto operator!=(
      const small_vector<elem_, size1_, unsafe1_> &a,
      const small_vector<elem_, size2_, unsafe2_> &b) noexcept
      -> bool;

  template <typename elem_,
            ptrdiff_t size1_,
            bool      unsafe1_,
            ptrdiff_t size2_,
            bool      unsafe2_>
  [[nodiscard]] auto operator<=(
      const small_vector<elem_, size1_, unsafe1_> &a,
      const small_vector<elem_, size2_, unsafe2_> &b) noexcept
      -> bool;

  template <typename elem_,
            ptrdiff_t size1_,
            bool      unsafe1_,
            ptrdiff_t size2_,
            bool      unsafe2_>
  [[nodiscard]] auto operator>=(
      const small_vector<elem_, size1_, unsafe1_> &a,
      const small_vector<elem_, size2_, unsafe2_> &b) noexcept
      -> bool;

  template <typename elem_,
            ptrdiff_t size1_,
            bool      unsafe1_,
            ptrdiff_t size2_,
            bool      unsafe2_>
  [[nodiscard]] auto operator<(
      const small_vector<elem_, size1_, unsafe1_> &a,
      const small_vector<elem_, size2_, unsafe2_> &b) noexcept
      -> bool;

  template <typename elem_,
            ptrdiff_t size1_,
            bool      unsafe1_,
            ptrdiff_t size2_,
            bool      unsafe2_>
  [[nodiscard]] auto operator>(
      const small_vector<elem_, size1_, unsafe1_> &a,
      const small_vector<elem_, size2_, unsafe2_> &b) noexcept
      -> bool;
}

#include "small_vector.impl.h"

#endif
