/*  Copyright (c) 2022 Mitya Selivanov
 */

#include "action.h"

#include "impact.h"

namespace laplace {
  time_type const action::default_tick_duration = 10;

  auto action::is_error() const noexcept -> bool {
    return m_is_error;
  }

  auto action::get_tick_duration() const noexcept -> time_type {
    return m_tick_duration;
  }

  auto action::setup(impact_generator gen) const noexcept -> action {
    return _bind([&]() {
      auto a        = action { *this };
      a.m_generator = std::move(gen);
      return a;
    });
  }

  auto action::set_tick_duration(time_type duration) const noexcept
      -> action {
    return _bind([&]() {
      if (duration <= 0)
        return _error();
      auto a            = action { *this };
      a.m_tick_duration = duration;
      return a;
    });
  }

  auto action::set_self(entity const &self) const noexcept -> action {
    return _bind([&]() {
      auto a   = action { *this };
      a.m_self = self;
      return a;
    });
  }

  auto action::run(entity self) const noexcept
      -> coro::generator<impact_list> {
    return !is_error() && m_generator ? m_generator(std::move(self))
                                      : _noop()(std::move(self));
  }

  auto action::get_self() const noexcept -> entity {
    return m_self;
  }

  auto action::_error() const noexcept -> action {
    auto a       = action { *this };
    a.m_is_error = true;
    return a;
  }

  auto action::_bind(std::function<action()> f) const noexcept
      -> action {
    if (is_error())
      return *this;
    return f();
  }

  auto action::_noop() noexcept -> impact_generator {
    return [](entity) -> coro::generator<impact_list> { co_return; };
  }
}
