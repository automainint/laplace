/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "scheduler.h"

#include "basic_impact.h"
#include "world.h"
#include <mutex>

namespace laplace::engine {
  using std::unique_lock, std::lock, std::adopt_lock, std::jthread,
      std::thread, std::function, std::ostringstream;

  sl::whole const scheduler::overthreading_limit = 8;
  sl::whole const scheduler::concurrency_limit   = 0x1000;

  scheduler::scheduler(world &w) noexcept : m_world(w) { }

  void scheduler::schedule(sl::whole const delta) noexcept {
    lock(m_lock_ex, m_lock_in);
    auto _ul_ex = unique_lock(m_lock_ex, adopt_lock);
    auto _ul    = unique_lock(m_lock_in, adopt_lock);

    m_tick_count += delta;
    _ul.unlock();

    m_sync.notify_all();
  }

  void scheduler::join() noexcept {
    lock(m_lock_ex, m_lock_in);
    auto _ul_ex = unique_lock(m_lock_ex, adopt_lock);
    auto _ul    = unique_lock(m_lock_in, adopt_lock);

    if (!m_threads.empty())
      m_sync.wait(_ul, [this] {
        return m_tick_count <= 0 || m_threads.empty();
      });
  }

  void scheduler::set_thread_count(
      sl::whole const thread_count) noexcept {
    lock(m_lock_ex, m_lock_in);
    auto _ul_ex = unique_lock(m_lock_ex, adopt_lock);
    auto _ul    = unique_lock(m_lock_in, adopt_lock);

    const auto thread_count_limit = []() -> sl::whole {
      const auto limit = thread::hardware_concurrency() *
                         overthreading_limit;
      if (limit < 0)
        return 0;
      if (limit > concurrency_limit)
        return concurrency_limit;
      return limit;
    }();

    auto count = thread_count;

    if (count < 0) {
      log(log_event::warning,
          fmt("Invalid thread count %d. Set to 0.", (int) count),
          "Engine/Scheduler");
      count = 0;
    }

    if (count > thread_count_limit) {
      log(log_event::warning,
          fmt("Invalid thread count %d. Max %d.", (int) count,
              (int) thread_count_limit),
          "Engine/Scheduler");
      count = thread_count_limit;
    }

    if (count == m_threads.size())
      return;

    if (!m_threads.empty()) {
      m_done = true;
      _ul.unlock();

      m_sync.notify_all();

      for (auto &t : m_threads) {
        try {
          t.join();
        } catch (std::system_error &error) {
          error_("Unable to join a thread.", __FUNCTION__);
          error_(error.what(), __FUNCTION__);
        }
      }

      _ul.lock();
      m_done = false;
    }

    m_threads.resize(count);

    for (auto &t : m_threads)
      for (int i = 0; i < 4; i++) {
        try {
          t = jthread([this] { this->tick_thread(); });
          break;
        } catch (std::system_error &error) {
          error_("Unable to create a thread.", __FUNCTION__);
          error_(error.what(), __FUNCTION__);
          std::this_thread::yield();
        }
      }
  }

  void scheduler::set_done() noexcept {
    lock(m_lock_ex, m_lock_in);
    auto _ul_ex = unique_lock(m_lock_ex, adopt_lock);
    auto _ul    = unique_lock(m_lock_in, adopt_lock);

    m_done = true;
    _ul.unlock();

    m_sync.notify_all();
  }

  auto scheduler::get_thread_count() noexcept -> sl::whole {
    auto _ul = unique_lock(m_lock_ex);
    return m_threads.size();
  }

  void scheduler::fence(function<void()> const &fn) noexcept {
    auto _ul = unique_lock(m_lock_in);

    m_in++;

    if (m_in >= m_threads.size()) {
      m_out = 0;
      _ul.unlock();

      fn();

      m_sync.notify_all();

      _ul.lock();
    } else {
      m_sync.wait(_ul, [this] { return m_in >= m_threads.size(); });
    }

    m_out++;

    if (m_out >= m_threads.size()) {
      m_in = 0;
      _ul.unlock();

      m_sync.notify_all();
    } else {
      m_sync.wait(_ul, [this] { return m_out >= m_threads.size(); });
    }
  }

  void scheduler::tick_thread() noexcept {
    auto _ul = unique_lock(m_lock_in);

    while (m_sync.wait(_ul,
                       [this] { return m_tick_count > 0 || m_done; }),
           !m_done) {
      while (m_tick_count > 0) {
        _ul.unlock();

        while (!m_world.no_queue()) {
          fence([&] {
            perform_sync_queue();
            m_world.clean_sync_queue();
          });

          perform_async_queue();
          fence([&] { m_world.clean_async_queue(); });
        }

        update_dynamic_entities();
        fence([&] { m_world.reset_index(); });

        adjust_all_entities();
        fence([&] {
          m_world.reset_index();
          m_tick_count--;
        });

        _ul.lock();
      }

      m_sync.notify_all();
    }
  }

  void scheduler::perform_sync_queue() noexcept {
    while (auto ev = m_world.next_sync_impact())
      ev->perform({ m_world, access::sync });
  }

  void scheduler::perform_async_queue() noexcept {
    while (auto ev = m_world.next_async_impact())
      ev->perform({ m_world, access::async });
  }

  void scheduler::update_dynamic_entities() noexcept {
    while (auto en = m_world.next_dynamic_entity())
      if (en->clock())
        en->tick({ m_world, access::async });
  }

  void scheduler::adjust_all_entities() noexcept {
    while (auto en = m_world.next_entity()) en->adjust();
  }
}
