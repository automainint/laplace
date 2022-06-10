/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_ACTION_H
#define LAPLACE_ACTION_H

#include "impact.h"
#include "options.h"
#include <coro/generator.h>
#include <functional>

namespace laplace {
  class action {
  public:
    static time_type const default_tick_duration;

    using impact_generator = std::function<coro::generator<impact>()>;

    [[nodiscard]] auto is_error() const noexcept -> bool;

    [[nodiscard]] auto get_tick_duration() const noexcept
        -> time_type;

    [[nodiscard]] auto setup(impact_generator gen) const noexcept
        -> action;

    [[nodiscard]] auto set_tick_duration(
        time_type duration) const noexcept -> action;

    [[nodiscard]] auto run() const noexcept
        -> coro::generator<impact>;

  private:
    [[nodiscard]] static auto _error() noexcept -> action;
    [[nodiscard]] static auto _noop() noexcept -> impact_generator;

    bool             m_is_error      = false;
    time_type        m_tick_duration = default_tick_duration;
    impact_generator m_generator     = {};
  };
}

#endif
