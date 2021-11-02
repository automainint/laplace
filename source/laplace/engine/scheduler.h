/*  laplace/engine/scheduler.h
 *
 *      The World multithreading scheduler. Full thread-safe.
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_scheduler_h
#define laplace_engine_scheduler_h

#include "world.predef.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace laplace::engine {
  class scheduler {
  public:
    static sl::whole const overthreading_limit;
    static sl::whole const concurrency_limit;

    scheduler(scheduler const &) = delete;
    scheduler(scheduler &&)      = delete;
    auto operator=(scheduler const &) -> scheduler & = delete;
    auto operator=(scheduler &&) -> scheduler & = delete;

    explicit scheduler(world &w) noexcept;
    ~scheduler() noexcept = default;

    void schedule(sl::whole delta) noexcept;
    void join() noexcept;

    void set_thread_count(sl::whole thread_count) noexcept;
    void set_done() noexcept;

    [[nodiscard]] auto get_thread_count() noexcept -> sl::whole;

  private:
    void fence(std::function<void()> const &fn) noexcept;
    void tick_thread() noexcept;

    void perform_sync_queue() noexcept;
    void perform_async_queue() noexcept;
    void update_dynamic_entities() noexcept;
    void adjust_all_entities() noexcept;

    world &m_world;

    std::mutex                m_lock_ex;
    std::mutex                m_lock_in;
    std::condition_variable   m_sync;
    std::vector<std::jthread> m_threads;

    bool      m_done       = false;
    sl::whole m_in         = 0;
    sl::whole m_out        = 0;
    sl::whole m_tick_count = 0;
  };
}

#endif
