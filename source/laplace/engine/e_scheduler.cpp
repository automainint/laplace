/*  laplace/engine/e_scheduler.cpp
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

#include "basic_impact.h"
#include "scheduler.h"
#include "world.h"
#include <mutex>

namespace laplace::engine {
  using std::unique_lock, std::lock, std::adopt_lock, std::jthread,
      std::function;

  scheduler::scheduler(ref_world w) : m_world(w) { }

  scheduler::~scheduler() {
    set_done();
  }

  void scheduler::schedule(size_t delta) {
    lock(m_lock_ex, m_lock_in);
    auto _ul_ex = unique_lock(m_lock_ex, adopt_lock);
    auto _ul    = unique_lock(m_lock_in, adopt_lock);

    m_tick_count += delta;
    _ul.unlock();

    m_sync.notify_all();
  }

  void scheduler::join() {
    lock(m_lock_ex, m_lock_in);
    auto _ul_ex = unique_lock(m_lock_ex, adopt_lock);
    auto _ul    = unique_lock(m_lock_in, adopt_lock);

    if (!m_threads.empty()) {
      m_sync.wait(_ul, [this] {
        return m_tick_count == 0u || m_threads.empty();
      });
    }
  }

  void scheduler::set_thread_count(size_t thread_count) {
    lock(m_lock_ex, m_lock_in);
    auto _ul_ex = unique_lock(m_lock_ex, adopt_lock);
    auto _ul    = unique_lock(m_lock_in, adopt_lock);

    const auto thread_count_limit = jthread::hardware_concurrency() *
                                    overthreading_limit;

    if (thread_count > thread_count_limit) {
      verb(fmt("Scheduler: Invalid thread count %zu (max %zu).",
               thread_count, thread_count_limit));

      thread_count = thread_count_limit;
    }

    size_t n = m_threads.size();

    if (thread_count < n) {
      m_done = true;
      _ul.unlock();

      m_sync.notify_all();

      for (auto &t : m_threads) { t.join(); }

      _ul.lock();
      m_done = false;

      n = 0;
    } else if (thread_count > n) {
      m_threads.resize(thread_count);
    }

    for (size_t i = n; i < m_threads.size(); i++) {
      m_threads[i] = jthread([this] {
        this->tick_thread();
      });
    }
  }

  auto scheduler::get_thread_count() -> size_t {
    auto _ul = unique_lock(m_lock_ex);
    return m_threads.size();
  }

  void scheduler::set_done() {
    lock(m_lock_ex, m_lock_in);
    auto _ul_ex = unique_lock(m_lock_ex, adopt_lock);
    auto _ul    = unique_lock(m_lock_in, adopt_lock);

    m_done = true;
    _ul.unlock();

    m_sync.notify_all();
  }

  void scheduler::sync(function<void()> fn) {
    auto _ul = unique_lock(m_lock_in);

    m_in++;

    if (m_in >= m_threads.size()) {
      m_out = 0;
      _ul.unlock();

      fn();

      m_sync.notify_all();

      _ul.lock();
    } else {
      m_sync.wait(_ul, [this] {
        return m_in >= m_threads.size();
      });
    }

    m_out++;

    if (m_out >= m_threads.size()) {
      m_in = 0;
      _ul.unlock();

      m_sync.notify_all();
    } else {
      m_sync.wait(_ul, [this] {
        return m_out >= m_threads.size();
      });
    }
  }

  void scheduler::tick_thread() {
    auto _ul = unique_lock(m_lock_in);

    while (m_sync.wait(_ul,
                       [this] {
                         return m_tick_count > 0 || m_done;
                       }),
           !m_done) {
      while (m_tick_count > 0) {
        _ul.unlock();

        while (!m_world.get().no_queue()) {
          /*  Execute the sync queue.
           */

          sync([this] {
            while (auto ev = m_world.get().next_sync_impact()) {
              ev->perform({ m_world, access::sync });
            }

            m_world.get().clean_sync_queue();
          });

          /*  Execute the async queue.
           */

          while (auto ev = m_world.get().next_async_impact()) {
            ev->perform({ m_world, access::async });
          }

          sync([this] {
            m_world.get().clean_async_queue();
          });
        }

        /*  Update the dynamic entities.
         */

        while (auto en = m_world.get().next_dynamic_entity()) {
          if (en->clock()) {
            en->tick({ m_world, access::async });
          }
        }

        sync([this] {
          m_world.get().reset_index();
        });

        /*  Adjust all the entities.
         */

        while (auto en = m_world.get().next_entity()) {
          en->adjust();
        }

        sync([this] {
          m_world.get().reset_index();

          m_tick_count--;
        });

        _ul.lock();
      }

      _ul.unlock();

      m_sync.notify_all();

      _ul.lock();
    }
  }
}
