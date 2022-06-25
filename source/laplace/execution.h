/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_EXECUTION_H
#define LAPLACE_EXECUTION_H

#include "action.h"
#include <thread>
#include <vector>

namespace laplace {
  class execution {
  public:
    static const ptrdiff_t default_thread_count;
    static const ptrdiff_t overthreading_limit;

    execution() noexcept;
    execution(execution const &exe) noexcept;
    execution(execution &&exe) noexcept;
    ~execution() noexcept;

    auto operator=(execution const &exe) noexcept -> execution &;
    auto operator=(execution &&exe) noexcept -> execution &;

    [[nodiscard]] auto is_error() const noexcept -> bool;

    [[nodiscard]] auto get_thread_count() const noexcept -> ptrdiff_t;

    [[nodiscard]] auto set_thread_count(
        ptrdiff_t thread_count) const noexcept -> execution;

    void queue(action a) noexcept;

    void schedule(time_type time) noexcept;
    void join() noexcept;

  private:
    [[nodiscard]] static auto _error() noexcept -> execution;

    void _assign(execution const &exe) noexcept;
    void _assign(execution &&exe) noexcept;
    void _set_error() noexcept;

    bool      m_is_error     = false;
    bool      m_is_done      = false;
    ptrdiff_t m_thread_count = default_thread_count;
    action    m_action;
  };
}

#endif
