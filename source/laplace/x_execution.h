/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_X_EXECUTION_H
#define LAPLACE_X_EXECUTION_H

#include "x_impact.h"
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
      ptrdiff_t                    order         = 0;
      time_type                    clock         = 0;
      time_type                    tick_duration = 0;
      coro::generator<impact_list> generator;
    };

    struct sync_impact {
      ptrdiff_t order = 0;
      impact    value;
    };

    enum tick_status { done, continue_ };

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
    void _add_sync(ptrdiff_t order, impact const &i) noexcept;
    void _add_async(impact const &i) noexcept;
    void _add_fork(ptrdiff_t order, action const &a) noexcept;
    [[nodiscard]] auto _add_impacts(ptrdiff_t          order,
                                    impact_list const &list) noexcept
        -> tick_status;
    [[nodiscard]] auto _perform(ptrdiff_t index) noexcept -> bool;
    [[nodiscard]] auto _next_async() noexcept
        -> std::optional<ptrdiff_t>;
    [[nodiscard]] auto _dec_tick() noexcept -> bool;
    [[nodiscard]] auto _tick_done() noexcept -> bool;
    void               _add_tick_done(bool done) noexcept;
    [[nodiscard]] auto _perform_actions() noexcept -> bool;
    void               _set_error_internal() noexcept;
    void               _apply_impacts() noexcept;
    void               _apply_time() noexcept;
    void               _thread() noexcept;
    void               _init_threads(ptrdiff_t thread_count) noexcept;
    void               _init() noexcept;

    void _assign(execution const &exe) noexcept;
    void _assign(execution &&exe) noexcept;
    void _set_error() noexcept;

    std::atomic_bool               m_is_error     = false;
    bool                           m_is_done      = true;
    bool                           m_is_tick_done = true;
    ptrdiff_t                      m_fence_in     = 0;
    ptrdiff_t                      m_fence_out    = 0;
    ptrdiff_t                      m_queue_index  = 0;
    ptrdiff_t                      m_async_index  = 0;
    std::pmr::vector<sync_impact>  m_sync;
    std::pmr::vector<impact>       m_async;
    std::pmr::vector<action_state> m_forks;
    time_type                      m_ticks = 0;
    std::mutex                     m_lock;
    std::condition_variable        m_on_tick;
    std::condition_variable        m_on_join;
    std::condition_variable        m_on_fence;

    state                          m_state;
    std::pmr::vector<action_state> m_queue;
    std::pmr::vector<std::jthread> m_threads;
  };
}

#endif
