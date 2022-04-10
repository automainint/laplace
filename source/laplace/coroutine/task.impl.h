/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_COROUTINE_TASK_IMPL_H
#define LAPLACE_COROUTINE_TASK_IMPL_H

namespace laplace::coroutine {
  template <typename return_type_>
  inline auto
  task<return_type_>::promise_type::initial_suspend() noexcept {
    return std::suspend_always {};
  }

  template <typename return_type_>
  inline auto
  task<return_type_>::promise_type::final_suspend() noexcept {
    return std::suspend_always {};
  }

  template <typename return_type_>
  inline auto
  task<return_type_>::promise_type::get_return_object() noexcept {
    return task { handle_type::from_promise(*this) };
  }

  template <typename return_type_>
  inline auto
  task<return_type_>::promise_type::unhandled_exception() noexcept {
    m_exception = std::current_exception();
  }

  template <typename return_type_>
  inline void task<return_type_>::promise_type::return_value(
      return_type_ const &_value) noexcept {
    m_value = _value;
  }

  template <typename return_type_>
  inline void task<return_type_>::promise_type::return_value(
      return_type_ &&_value) noexcept {
    m_value = std::move(_value);
  }

  template <typename return_type_>
  inline auto task<return_type_>::promise_type::yield_value(
      std::default_sentinel_t) noexcept {
    return std::suspend_always {};
  }

  template <typename return_type_>
  inline task<return_type_>::task(task const &other) noexcept :
      m_handle(other.m_handle) {
    _acquire_handle(m_handle);
  }

  template <typename return_type_>
  inline task<return_type_>::task(task &&other) noexcept :
      m_handle(std::move(other.m_handle)) {
    other.m_handle = nullptr;
  }

  template <typename return_type_>
  inline auto task<return_type_>::operator=(
      task const &other) noexcept -> task & {
    _release_handle(m_handle);
    m_handle = other.m_handle;
    _acquire_handle(m_handle);
    return *this;
  }

  template <typename return_type_>
  inline auto task<return_type_>::operator=(task &&other) noexcept
      -> task & {
    _release_handle(m_handle);
    m_handle       = std::move(other.m_handle);
    other.m_handle = nullptr;
    return *this;
  }

  template <typename return_type_>
  inline task<return_type_>::task(handle_type handle) noexcept :
      m_handle(handle) {
    _acquire_handle(m_handle);
  }

  template <typename return_type_>
  inline task<return_type_>::~task() noexcept {
    _release_handle(m_handle);
  }

  template <typename return_type_>
  inline auto task<return_type_>::await_ready() const noexcept {
    return _is_done(m_handle);
  }

  template <typename return_type_>
  inline auto task<return_type_>::await_suspend(
      std::coroutine_handle<>) noexcept {
    _resume(m_handle);
    return false;
  }

  template <typename return_type_>
  inline auto task<return_type_>::await_suspend(
      handle_type handle) noexcept {
    _nest(handle, m_handle);
    _resume(m_handle);
    return false;
  }

  template <typename return_type_>
  inline auto task<return_type_>::await_resume() {
    return _get_value(m_handle);
  }

  template <typename return_type_>
  inline void task<return_type_>::resume() noexcept {
    _resume(m_handle);
  }

  template <typename return_type_>
  inline auto task<return_type_>::run() {
    _resume(m_handle);
    return _get_value(m_handle);
  }

  template <typename return_type_>
  inline auto task<return_type_>::get() noexcept {
    return _get_value(m_handle);
  }

  template <typename return_type_>
  inline auto task<return_type_>::is_done() const noexcept {
    return _is_done(m_handle);
  }

  inline auto task<void>::promise_type::initial_suspend() noexcept {
    return std::suspend_always {};
  }

  inline auto task<void>::promise_type::final_suspend() noexcept {
    return std::suspend_always {};
  }

  inline auto task<void>::promise_type::get_return_object() noexcept {
    return task { handle_type::from_promise(*this) };
  }

  inline auto
  task<void>::promise_type::unhandled_exception() noexcept {
    m_exception = std::current_exception();
  }

  inline void task<void>::promise_type::return_void() noexcept { }

  inline auto task<void>::promise_type::yield_value(
      std::default_sentinel_t) noexcept {
    return std::suspend_always {};
  }

  inline task<void>::task(task const &other) noexcept :
      m_handle(other.m_handle) {
    _acquire_handle(m_handle);
  }

  inline task<void>::task(task &&other) noexcept :
      m_handle(std::move(other.m_handle)) {
    other.m_handle = nullptr;
  }

  inline auto task<void>::operator=(task const &other) noexcept
      -> task & {
    _release_handle(m_handle);
    m_handle = other.m_handle;
    _acquire_handle(m_handle);
    return *this;
  }

  inline auto task<void>::operator=(task &&other) noexcept -> task & {
    _release_handle(m_handle);
    m_handle       = std::move(other.m_handle);
    other.m_handle = nullptr;
    return *this;
  }

  inline task<void>::task(handle_type handle) noexcept :
      m_handle(handle) {
    _acquire_handle(m_handle);
  }

  inline task<void>::~task() noexcept {
    _release_handle(m_handle);
  }

  inline auto task<void>::await_ready() const noexcept {
    return _is_done(m_handle);
  }

  inline auto task<void>::await_resume() {
    _get_value(m_handle);
  }

  inline auto task<void>::await_suspend(
      std::coroutine_handle<>) noexcept {
    _resume(m_handle);
    return false;
  }

  inline auto task<void>::await_suspend(handle_type handle) noexcept {
    _nest(handle, m_handle);
    _resume(m_handle);
    return false;
  }

  inline void task<void>::resume() {
    _resume(m_handle);
  }

  inline auto task<void>::is_done() const noexcept {
    return _is_done(m_handle);
  }
}

#endif
