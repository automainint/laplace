/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_COROUTINE_SENTINEL_IMPL_H
#define LAPLACE_COROUTINE_SENTINEL_IMPL_H

namespace laplace::coroutine {
  constexpr auto sentinel_t::await_ready() const noexcept {
    return false;
  }

  constexpr auto sentinel_t::await_suspend(
      std::coroutine_handle<>) const noexcept {
    return true;
  }

  constexpr auto sentinel_t::await_resume() const noexcept { }
}

#endif
