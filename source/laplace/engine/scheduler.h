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
#include <thread>

namespace laplace::engine {
  /*  TODO
   *  Atomics, jthread.
   */
  class scheduler {
  public:
    static constexpr size_t overthreading_limit = 8;

    scheduler(const scheduler &) = delete;
    auto operator=(const scheduler &) -> scheduler & = delete;

    scheduler(ref_world w);
    ~scheduler();

    void schedule(size_t delta);
    void join();

    void               set_thread_count(size_t thread_count);
    [[nodiscard]] auto get_thread_count() -> size_t;

  private:
    void set_done();
    void sync(std::function<void()> fn);
    void tick_thread();

    std::reference_wrapper<world> m_world;

    std::mutex               m_lock;
    std::mutex               m_extra_lock;
    std::condition_variable  m_sync;
    std::vector<std::thread> m_threads;

    bool   m_done       = false;
    size_t m_in         = 0;
    size_t m_out        = 0;
    size_t m_tick_count = 0;
  };
}

#endif
