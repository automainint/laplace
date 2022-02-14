/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_COROUTINE_ITERATOR_H
#define LAPLACE_COROUTINE_ITERATOR_H

#include "utils.h"
#include <iterator>

namespace laplace::coroutine {
  template <promise_shared promise_>
  struct iterator {
    iterator(iterator const &) = delete;
    iterator &operator=(iterator const &) = delete;
    iterator &operator=(iterator &&) = delete;

    iterator(iterator &&other) noexcept;
    explicit iterator(
        std::coroutine_handle<promise_> handle) noexcept;
    ~iterator() noexcept;

    void               operator++() noexcept;
    [[nodiscard]] auto operator*();
    [[nodiscard]] auto operator!=(std::default_sentinel_t) noexcept;

  private:
    std::coroutine_handle<promise_> m_handle;
  };
}

#include "iterator.impl.h"

#endif
