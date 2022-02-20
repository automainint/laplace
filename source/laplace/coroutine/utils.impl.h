/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_COROUTINE_UTILS_IMPL_H
#define LAPLACE_COROUTINE_UTILS_IMPL_H

namespace laplace::coroutine {
  template <promise_not_shared promise_>
  inline void _acquire_handle(
      std::coroutine_handle<promise_>) noexcept { }

  template <promise_shared promise_>
  inline void _acquire_handle(
      std::coroutine_handle<promise_> handle) noexcept {
    if (handle)
      handle.promise().m_ref_count++;
  }

  template <promise_not_shared promise_>
  inline void _release_handle(
      std::coroutine_handle<promise_>) noexcept { }

  template <promise_shared promise_>
    requires(!promise_nested<promise_>)
  inline void _release_handle(
      std::coroutine_handle<promise_> handle) noexcept {
    if (!handle)
      return;
    if (--handle.promise().m_ref_count != 0)
      return;
    handle.destroy();
  }

  template <promise_nested promise_>
  inline void _release_handle(
      std::coroutine_handle<promise_> handle) noexcept {
    if (!handle)
      return;
    if (--handle.promise().m_ref_count != 0)
      return;
    _release_handle(handle.promise().m_pending);
    handle.destroy();
  }

  template <promise_returning promise_>
    requires(!promise_nested<promise_>)
  inline auto _resume(std::coroutine_handle<promise_> handle) noexcept
      -> bool {
    if (!handle || handle.done())
      return false;
    handle.resume();
    return true;
  }

  template <promise_void promise_>
    requires(!promise_nested<promise_>)
  inline auto _resume(std::coroutine_handle<promise_> handle)
      -> bool {
    if (!handle || handle.done())
      return false;
    handle.resume();
    if (handle.promise().m_exception)
      std::rethrow_exception(handle.promise().m_exception);
    return true;
  }

  template <promise_nested promise_>
  inline auto _resume(std::coroutine_handle<promise_> handle)
      -> bool {
    if (!handle || handle.done())
      return false;
    if (handle.promise().m_pending) {
      if (_resume(handle.promise().m_pending))
        return true;
      _release_handle(handle.promise().m_pending);
      handle.promise().m_pending = nullptr;
    }
    handle.resume();
    if (handle.promise().m_exception)
      std::rethrow_exception(handle.promise().m_exception);
    return true;
  }

  template <promise_basic promise_>
  inline auto _is_done(
      std::coroutine_handle<promise_> handle) noexcept {
    return handle && handle.done();
  }

  template <promise_returning promise_>
    requires(!promise_nested<promise_>)
  inline auto _get_value(std::coroutine_handle<promise_> handle) {
    if (!handle)
      return decltype(handle.promise().m_value) {};
    if (handle.promise().m_exception)
      std::rethrow_exception(handle.promise().m_exception);
    return std::move(handle.promise().m_value);
  }

  template <promise_returning promise_>
    requires(promise_nested<promise_>)
  inline auto _get_value(std::coroutine_handle<promise_> handle) {
    if (!handle)
      return decltype(handle.promise().m_value) {};
    if (handle.promise().m_pending)
      return _get_value(handle.promise().m_pending);
    if (handle.promise().m_exception)
      std::rethrow_exception(handle.promise().m_exception);
    return std::move(handle.promise().m_value);
  }

  template <promise_void promise_>
  inline auto _get_value(std::coroutine_handle<promise_> handle) {
    if (!handle)
      return;
    if (handle.promise().m_exception)
      std::rethrow_exception(handle.promise().m_exception);
  }

  template <promise_nested upper_, typename under_>
  inline void _nest(std::coroutine_handle<upper_> upper,
                    std::coroutine_handle<under_> under) {
    _release_handle(upper.promise().m_pending);
    upper.promise().m_pending = under;
    _acquire_handle(upper.promise().m_pending);
  }

  template <typename upper_, typename under_>
    requires(!promise_nested<upper_>)
  inline void _nest(std::coroutine_handle<upper_>,
                    std::coroutine_handle<under_>) { }
}

#endif
