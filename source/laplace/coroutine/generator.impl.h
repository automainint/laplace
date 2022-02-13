/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_COROUTINE_GENERATOR_IMPL_H
#define LAPLACE_COROUTINE_GENERATOR_IMPL_H

namespace laplace::coroutine {
  template <typename return_type_>
  inline auto
  generator<return_type_>::promise_type::initial_suspend() noexcept {
    return std::suspend_always {};
  }

  template <typename return_type_>
  inline auto
  generator<return_type_>::promise_type::final_suspend() noexcept {
    return std::suspend_always {};
  }

  template <typename return_type_>
  inline auto generator<return_type_>::promise_type::yield_value(
      return_type_ const &_value) noexcept {

    value = _value;
    return std::suspend_always {};
  }

  template <typename return_type_>
  inline auto generator<return_type_>::promise_type::yield_value(
      return_type_ &&_value) noexcept {
    value = std::move(_value);
    return std::suspend_always {};
  }

  template <typename return_type_>
  inline auto generator<return_type_>::promise_type::return_value(
      return_type_ const &_value) noexcept {
    value = _value;
    return std::suspend_always {};
  }

  template <typename return_type_>
  inline auto generator<return_type_>::promise_type::return_value(
      return_type_ &&_value) noexcept {
    value = std::move(_value);
    return std::suspend_always {};
  }

  template <typename return_type_>
  inline auto generator<
      return_type_>::promise_type::get_return_object() noexcept {
    return generator { handle_type::from_promise(*this) };
  }

  template <typename return_type_>
  inline auto generator<
      return_type_>::promise_type::unhandled_exception() noexcept {
    exception = std::current_exception();
  }

  template <typename return_type_>
  inline void generator<return_type_>::promise_type::free_handle(
      handle_type &handle) noexcept {
    if (!handle)
      return;
    handle.promise().ref_count--;
    if (handle.promise().ref_count == 0)
      handle.destroy();
  }

  template <typename return_type_>
  inline generator<return_type_>::iterator::iterator(
      iterator &&other) noexcept {
    m_handle       = std::move(other.m_handle);
    other.m_handle = nullptr;
  }

  template <typename return_type_>
  inline auto generator<return_type_>::iterator::operator=(
      iterator &&other) noexcept -> iterator & {
    promise_type::free_handle(m_handle);
    m_handle       = std::move(other.m_handle);
    other.m_handle = nullptr;
    return *this;
  }

  template <typename return_type_>
  inline generator<return_type_>::iterator::iterator(
      handle_type handle) noexcept :
      m_handle(handle) {
    m_handle.promise().ref_count++;
  }

  template <typename return_type_>
  inline generator<return_type_>::iterator::~iterator() noexcept {
    promise_type::free_handle(m_handle);
  }

  template <typename return_type_>
  inline auto
  generator<return_type_>::iterator::operator++() noexcept {
    if (!m_handle.done())
      m_handle.resume();
  }

  template <typename return_type_>
  inline auto generator<return_type_>::iterator::operator*() {
    if (m_handle.promise().exception)
      std::rethrow_exception(m_handle.promise().exception);
    return std::move(m_handle.promise().value);
  }

  template <typename return_type_>
  inline auto generator<return_type_>::iterator::operator!=(
      std::default_sentinel_t) noexcept {
    return !m_handle.done();
  }

  template <typename return_type_>
  inline generator<return_type_>::generator(
      generator const &other) noexcept {
    m_handle = other.m_handle;
    if (m_handle)
      m_handle.promise().ref_count++;
  }

  template <typename return_type_>
  inline generator<return_type_>::generator(
      generator &&other) noexcept {
    m_handle       = std::move(other.m_handle);
    other.m_handle = nullptr;
  }

  template <typename return_type_>
  inline auto generator<return_type_>::operator=(
      generator const &other) noexcept -> generator & {

    promise_type::free_handle(m_handle);
    m_handle = other.m_handle;
    if (m_handle)
      m_handle.promise().ref_count++;
    return *this;
  }

  template <typename return_type_>
  inline auto generator<return_type_>::operator=(
      generator &&other) noexcept -> generator & {
    promise_type::free_handle(m_handle);
    m_handle       = std::move(other.m_handle);
    other.m_handle = nullptr;
    return *this;
  }

  template <typename return_type_>
  inline generator<return_type_>::generator(
      handle_type handle) noexcept :
      m_handle(handle) {
    m_handle.promise().ref_count++;
  }

  template <typename return_type_>
  inline generator<return_type_>::~generator() noexcept {
    promise_type::free_handle(m_handle);
  }

  template <typename return_type_>
  inline auto generator<return_type_>::await_ready() const noexcept {
    return is_done();
  }

  template <typename return_type_>
  inline auto generator<return_type_>::await_suspend(
      std::coroutine_handle<> handle) noexcept {
    resume();
    return false;
  }

  template <typename return_type_>
  inline auto generator<return_type_>::await_resume() {
    if (m_handle.promise().exception)
      std::rethrow_exception(m_handle.promise().exception);
    return std::move(m_handle.promise().value);
  }

  template <typename return_type_>
  inline auto generator<return_type_>::begin() noexcept {
    resume();
    return iterator { m_handle };
  }

  template <typename return_type_>
  inline auto generator<return_type_>::end() noexcept {
    return std::default_sentinel;
  }

  template <typename return_type_>
  inline auto generator<return_type_>::get() {
    resume();
    if (m_handle.promise().exception)
      std::rethrow_exception(m_handle.promise().exception);
    return std::move(m_handle.promise().value);
  }

  template <typename return_type_>
  inline auto generator<return_type_>::is_done() const noexcept {
    return m_handle.done();
  }

  template <typename return_type_>
  inline void generator<return_type_>::resume() noexcept {
    if (!m_handle.done())
      m_handle.resume();
  }
}

#endif
