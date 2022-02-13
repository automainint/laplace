/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_COROUTINE_SENTINEL_H
#define LAPLACE_COROUTINE_SENTINEL_H

#include <coroutine>

namespace laplace::coroutine {
  struct sentinel_t {
    constexpr auto await_ready() const noexcept;
    constexpr auto await_suspend(
        std::coroutine_handle<>) const noexcept;
    constexpr auto await_resume() const noexcept;
  };

  /*  Suspend current coroutine and return control to the caller.
   *  co_await sentinel;
   */
  static constexpr auto sentinel = sentinel_t {};
}

#include "sentinel.impl.h"

#endif
