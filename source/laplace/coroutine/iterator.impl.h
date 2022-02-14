/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_COROUTINE_ITERATOR_IMPL_H
#define LAPLACE_COROUTINE_ITERATOR_IMPL_H

namespace laplace::coroutine {
  template <promise_shared promise_>
  inline iterator<promise_>::iterator(iterator &&other) noexcept :
      m_handle(std::move(other.m_handle)) {
    other.m_handle = nullptr;
  }

  template <promise_shared promise_>
  inline iterator<promise_>::iterator(
      std::coroutine_handle<promise_> handle) noexcept :
      m_handle(handle) {
    _acquire_handle(m_handle);
  }

  template <promise_shared promise_>
  inline iterator<promise_>::~iterator() noexcept {
    _release_handle(m_handle);
  }

  template <promise_shared promise_>
  inline void iterator<promise_>::operator++() noexcept {
    _resume(m_handle);
  }

  template <promise_shared promise_>
  inline auto iterator<promise_>::operator*() {
    return _get_value(m_handle);
  }

  template <promise_shared promise_>
  inline auto iterator<promise_>::operator!=(
      std::default_sentinel_t) noexcept {
    return !_is_done(m_handle);
  }
}

#endif
