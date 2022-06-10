/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "action.h"

namespace laplace {
  using std::function;

  time_type const action::default_tick_duration = 10;

  auto action::is_error() const noexcept -> bool {
    return m_is_error;
  }

  auto action::get_tick_duration() const noexcept -> time_type {
    return m_tick_duration;
  }

  auto action::setup(impact_generator gen) const noexcept -> action {
    if (is_error())
      return *this;
    
    auto a        = action { *this };
    a.m_generator = std::move(gen);
    return a;
  }

  auto action::set_tick_duration(time_type duration) const noexcept
      -> action {
    if (is_error())
      return *this;
    if (duration <= 0)
      return _error();

    auto a            = action { *this };
    a.m_tick_duration = duration;
    return a;
  }

  auto action::run() const noexcept -> coro::generator<impact> {
    return !is_error() && m_generator ? m_generator() : _noop()();
  }

  auto action::_error() noexcept -> action {
    auto a       = action {};
    a.m_is_error = true;
    return a;
  }

  auto action::_noop() noexcept -> impact_generator {
    return []() -> coro::generator<impact> { co_return; };
  }
}
