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
  inline auto task<return_type_>::promise_type::return_value(
      return_type_ const &_value) noexcept {
    value = _value;
    return std::suspend_always {};
  }

  template <typename return_type_>
  inline auto task<return_type_>::promise_type::return_value(
      return_type_ &&_value) noexcept {
    value = std::move(_value);
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
    exception = std::current_exception();
  }

  template <typename return_type_>
  inline task<return_type_>::task(task const &other) noexcept {
    m_handle = other.m_handle;
    if (m_handle)
      m_handle.promise().ref_count++;
  }

  template <typename return_type_>
  inline task<return_type_>::task(task &&other) noexcept {
    m_handle       = std::move(other.m_handle);
    other.m_handle = nullptr;
  }

  template <typename return_type_>
  inline auto task<return_type_>::operator=(
      task const &other) noexcept -> task & {
    free_handle();
    m_handle = other.m_handle;
    if (m_handle)
      m_handle.promise().ref_count++;
    return *this;
  }

  template <typename return_type_>
  inline auto task<return_type_>::operator=(task &&other) noexcept
      -> task & {
    free_handle();
    m_handle       = std::move(other.m_handle);
    other.m_handle = nullptr;
    return *this;
  }

  template <typename return_type_>
  inline task<return_type_>::task(handle_type handle) noexcept :
      m_handle(handle) {
    m_handle.promise().ref_count++;
  }

  template <typename return_type_>
  inline task<return_type_>::~task() noexcept {
    free_handle();
  }

  template <typename return_type_>
  inline auto task<return_type_>::await_ready() const noexcept {
    return m_handle.done();
  }

  template <typename return_type_>
  inline auto task<return_type_>::await_suspend(
      std::coroutine_handle<> handle) noexcept {
    if (!m_handle.done())
      m_handle.resume();
    return false;
  }

  template <typename return_type_>
  inline auto task<return_type_>::await_resume() {
    if (m_handle.promise().exception)
      std::rethrow_exception(m_handle.promise().exception);
    return std::move(m_handle.promise().value);
  }

  template <typename return_type_>
  inline void task<return_type_>::resume() noexcept {
    if (!m_handle.done())
      m_handle.resume();
  }

  template <typename return_type_>
  inline auto task<return_type_>::get() {
    if (!m_handle.done())
      m_handle.resume();
    if (m_handle.promise().exception)
      std::rethrow_exception(m_handle.promise().exception);
    return std::move(m_handle.promise().value);
  }

  template <typename return_type_>
  inline auto task<return_type_>::is_done() const noexcept {
    return m_handle.done();
  }

  template <typename return_type_>
  inline void task<return_type_>::free_handle() noexcept {
    if (!m_handle)
      return;
    m_handle.promise().ref_count--;
    if (m_handle.promise().ref_count == 0)
      m_handle.destroy();
  }

  inline auto task<void>::promise_type::initial_suspend() noexcept {
    return std::suspend_always {};
  }

  inline auto task<void>::promise_type::final_suspend() noexcept {
    return std::suspend_always {};
  }

  inline auto task<void>::promise_type::return_void() noexcept {
    return std::suspend_always {};
  }

  inline auto task<void>::promise_type::get_return_object() noexcept {
    return task { handle_type::from_promise(*this) };
  }

  inline auto
  task<void>::promise_type::unhandled_exception() noexcept {
    exception = std::current_exception();
  }

  inline task<void>::task(task const &other) noexcept {
    m_handle = other.m_handle;
    if (m_handle)
      m_handle.promise().ref_count++;
  }

  inline task<void>::task(task &&other) noexcept {
    m_handle       = std::move(other.m_handle);
    other.m_handle = nullptr;
  }

  inline auto task<void>::operator=(task const &other) noexcept
      -> task & {
    free_handle();
    m_handle = other.m_handle;
    if (m_handle)
      m_handle.promise().ref_count++;
    return *this;
  }

  inline auto task<void>::operator=(task &&other) noexcept -> task & {
    free_handle();
    m_handle       = std::move(other.m_handle);
    other.m_handle = nullptr;
    return *this;
  }

  inline task<void>::task(handle_type handle) noexcept :
      m_handle(handle) {
    m_handle.promise().ref_count++;
  }

  inline task<void>::~task() noexcept {
    free_handle();
  }

  inline auto task<void>::await_ready() const noexcept {
    return m_handle.done();
  }

  inline auto task<void>::await_suspend(
      std::coroutine_handle<> handle) noexcept {
    if (!m_handle.done())
      m_handle.resume();
    return false;
  }

  inline auto task<void>::await_resume() {
    if (m_handle.promise().exception)
      std::rethrow_exception(m_handle.promise().exception);
  }

  inline void task<void>::resume() {
    if (!m_handle.done())
      m_handle.resume();
    if (m_handle.promise().exception)
      std::rethrow_exception(m_handle.promise().exception);
  }

  inline void task<void>::free_handle() noexcept {
    if (!m_handle)
      return;
    m_handle.promise().ref_count--;
    if (m_handle.promise().ref_count == 0)
      m_handle.destroy();
  }

  inline auto task<void>::is_done() const noexcept {
    return m_handle.done();
  }
}

#endif
