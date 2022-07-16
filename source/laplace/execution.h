/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_EXECUTION_H
#define LAPLACE_EXECUTION_H

#include "impact.h"
#include <atomic>
#include <condition_variable>
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
    struct action_state {
      ptrdiff_t                    index         = 0;
      time_type                    clock         = 0;
      time_type                    tick_duration = 0;
      coro::generator<impact_list> generator;
    };

    [[nodiscard]] auto _error() const noexcept -> execution;
    [[nodiscard]] auto _bind(
        std::function<execution()> f) const noexcept -> execution;
    template <typename type_>
    [[nodiscard]] auto _bind(std::function<type_()> f,
                             type_ def) const noexcept -> type_;

    void               _once(std::function<void()> f) noexcept;
    void               _done_and_notify() noexcept;
    void               _stop_threads() noexcept;
    [[nodiscard]] auto _next_action() noexcept
        -> std::optional<ptrdiff_t>;
    void               _add_sync(impact const &i) noexcept;
    void               _add_async(impact const &i) noexcept;
    void               _add_impacts(impact_list list) noexcept;
    [[nodiscard]] auto _next_async() noexcept
        -> std::optional<ptrdiff_t>;
    [[nodiscard]] auto _dec_tick() noexcept -> bool;
    void               _thread() noexcept;
    void               _init_threads(ptrdiff_t thread_count) noexcept;

    void _assign(execution const &exe) noexcept;
    void _assign(execution &&exe) noexcept;
    void _set_error() noexcept;

    bool                    m_is_error    = false;
    bool                    m_is_done     = true;
    ptrdiff_t               m_fence_in    = 0;
    ptrdiff_t               m_fence_out   = 0;
    ptrdiff_t               m_queue_index = 0;
    ptrdiff_t               m_async_index = 0;
    impact_list             m_sync;
    impact_list             m_async;
    time_type               m_ticks = 0;
    std::mutex              m_lock;
    std::condition_variable m_on_tick;
    std::condition_variable m_on_join;
    std::condition_variable m_on_fence;

    state                          m_state;
    std::pmr::vector<action_state> m_queue;
    std::pmr::vector<std::jthread> m_threads;
  };
}

#endif
