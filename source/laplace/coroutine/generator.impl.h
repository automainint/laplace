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
  template <typename type_>
  inline auto
  generator<type_>::promise_type::initial_suspend() noexcept {
    return std::suspend_always {};
  }

  template <typename type_>
  inline auto
  generator<type_>::promise_type::final_suspend() noexcept {
    return std::suspend_always {};
  }

  template <typename type_>
  inline auto
  generator<type_>::promise_type::get_return_object() noexcept {
    return generator { handle_type::from_promise(*this) };
  }

  template <typename type_>
  inline auto
  generator<type_>::promise_type::unhandled_exception() noexcept {
    m_exception = std::current_exception();
  }

  template <typename type_>
  inline void generator<type_>::promise_type::return_value(
      type_ const &value) noexcept {
    m_value = value;
  }

  template <typename type_>
  inline void generator<type_>::promise_type::return_value(
      type_ &&value) noexcept {
    m_value = std::move(value);
  }

  template <typename type_>
  inline auto generator<type_>::promise_type::yield_value(
      type_ const &value) noexcept {
    m_value = value;
    return std::suspend_always {};
  }

  template <typename type_>
  inline auto generator<type_>::promise_type::yield_value(
      type_ &&value) noexcept {
    m_value = std::move(value);
    return std::suspend_always {};
  }

  template <typename type_>
  inline auto generator<type_>::promise_type::yield_value(
      generator<type_> gen) noexcept {
    _nest(handle_type::from_promise(*this), gen.m_handle);
    _resume(gen.m_handle);
    return std::suspend_never {};
  }

  template <typename type_>
  inline generator<type_>::generator(generator const &other) noexcept
      :
      m_handle(other.m_handle) {
    _acquire_handle(m_handle);
  }

  template <typename type_>
  inline generator<type_>::generator(generator &&other) noexcept :
      m_handle(std::move(other.m_handle)) {
    other.m_handle = nullptr;
  }

  template <typename type_>
  inline auto generator<type_>::operator=(
      generator const &other) noexcept -> generator & {
    _release_handle(m_handle);
    m_handle = other.m_handle;
    _acquire_handle(m_handle);
    return *this;
  }

  template <typename type_>
  inline auto generator<type_>::operator=(generator &&other) noexcept
      -> generator & {
    _release_handle(m_handle);
    m_handle       = std::move(other.m_handle);
    other.m_handle = nullptr;
    return *this;
  }

  template <typename type_>
  inline generator<type_>::generator(handle_type handle) noexcept :
      m_handle(handle) {
    _acquire_handle(m_handle);
  }

  template <typename type_>
  inline generator<type_>::~generator() noexcept {
    _release_handle(m_handle);
  }

  template <typename type_>
  inline auto generator<type_>::await_ready() noexcept {
    return _is_done(m_handle);
  }

  template <typename type_>
  inline auto generator<type_>::await_suspend(
      std::coroutine_handle<>) noexcept {
    _resume(m_handle);
    return false;
  }

  template <typename type_>
  inline auto generator<type_>::await_resume() {
    return _get_value(m_handle);
  }

  template <typename type_>
  inline auto generator<type_>::begin() noexcept {
    _resume(m_handle);
    return iterator<promise_type> { m_handle };
  }

  template <typename type_>
  inline auto generator<type_>::end() noexcept {
    return std::default_sentinel;
  }

  template <typename type_>
  inline auto generator<type_>::next() {
    _resume(m_handle);
    return _get_value(m_handle);
  }

  template <typename type_>
  inline auto generator<type_>::get() noexcept {
    return _get_value(m_handle);
  }

  template <typename type_>
  inline auto generator<type_>::is_done() noexcept {
    return _is_done(m_handle);
  }
}

#endif
