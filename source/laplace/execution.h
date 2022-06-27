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

    [[nodiscard]] auto set_state(state s) const noexcept -> execution;

    [[nodiscard]] auto read_only() const noexcept -> access;

    void queue(action a) noexcept;

    void schedule(time_type time) noexcept;
    void join() noexcept;

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

    bool      m_is_error     = false;
    bool      m_is_done      = false;
    ptrdiff_t m_thread_count = default_thread_count;
    action    m_action;
    state     m_state;
  };
}

#endif
