/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_EXECUTION_H
#define LAPLACE_EXECUTION_H

#include "impact.h"
#include <thread>
#include <vector>

namespace laplace {
  class execution {
  public:
    execution() noexcept;
    explicit execution(std::pmr::memory_resource *resource) noexcept;
    execution(execution const &exe) noexcept;
    execution(execution &&exe) noexcept;
    ~execution() noexcept;

    auto operator=(execution const &exe) noexcept -> execution &;
    auto operator=(execution &&exe) noexcept -> execution &;

    [[nodiscard]] auto error() const noexcept -> bool;

    [[nodiscard]] auto thread_count() const noexcept -> ptrdiff_t;

    [[nodiscard]] auto set_thread_count(
        ptrdiff_t thread_count) const noexcept -> execution;

    [[nodiscard]] auto set_state(state s) const noexcept -> execution;

    [[nodiscard]] auto read_only() const noexcept -> access;

    void queue(action const &a) noexcept;

    void schedule(time_type time) noexcept;
    void join() noexcept;

    void schedule_and_join(time_type time) noexcept;

  private:
    [[nodiscard]] auto _error() const noexcept -> execution;
    [[nodiscard]] auto _bind(
        std::function<execution()> f) const noexcept -> execution;
    template <typename type_>
    [[nodiscard]] auto _bind(std::function<type_()> f,
                             type_ def) const noexcept -> type_;

    void _assign(execution const &exe) noexcept;
    void _assign(execution &&exe) noexcept;
    void _set_error() noexcept;

    struct action_state {
      ptrdiff_t                    index         = 0;
      time_type                    clock         = 0;
      time_type                    tick_duration = 0;
      coro::generator<impact_list> generator;
    };

    bool      m_is_error     = false;
    bool      m_is_done      = false;
    ptrdiff_t m_thread_count = default_thread_count;
    std::pmr::vector<action_state> m_queue;
    state                          m_state;
  };
}

#endif
