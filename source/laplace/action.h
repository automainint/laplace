/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_ACTION_H
#define LAPLACE_ACTION_H

#include "entity.h"
#include "options.h"
#include <coro/generator.h>
#include <functional>

namespace laplace {
  class impact_list;

  class action {
  public:
    static time_type const default_tick_duration;

    using impact_generator =
        std::function<coro::generator<impact_list>(entity self)>;

    [[nodiscard]] auto is_error() const noexcept -> bool;

    [[nodiscard]] auto get_tick_duration() const noexcept
        -> time_type;

    [[nodiscard]] auto setup(impact_generator gen) const noexcept
        -> action;

    [[nodiscard]] auto set_tick_duration(
        time_type duration) const noexcept -> action;

    [[nodiscard]] auto set_self(entity const &self) const noexcept
        -> action;

    [[nodiscard]] auto run(entity self) const noexcept
        -> coro::generator<impact_list>;

    [[nodiscard]] auto get_self() const noexcept -> entity;

  private:
    [[nodiscard]] auto _error() const noexcept -> action;
    [[nodiscard]] auto _bind(std::function<action()> f) const noexcept
        -> action;
    [[nodiscard]] static auto _noop() noexcept -> impact_generator;

    bool             m_is_error      = false;
    time_type        m_tick_duration = default_tick_duration;
    impact_generator m_generator     = {};
    entity           m_self          = {};
  };
}

#endif
